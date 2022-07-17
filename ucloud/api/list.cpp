#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <ufile-cppsdk/common.h>
#include <ufile-cppsdk/config.h>
#include <ufile-cppsdk/list.h>
#include <ufile-cppsdk/digest.h>
#include <ufile-cppsdk/errno.h>
#include <ufile-cppsdk/http.h>
#include <ufile-cppsdk/urlcodec.h>

#define LIST_MAX_COUNT (1000)

using namespace ucloud::cppsdk::http;
using namespace ucloud::cppsdk::utils;
using namespace ucloud::cppsdk::error;
using namespace ucloud::cppsdk::digest;
using namespace ucloud::cppsdk::config;

namespace ucloud {
namespace cppsdk {
namespace api {

UFileList::UFileList() : delimiter_("") {}

UFileList::~UFileList() {}

int UFileList::List(const std::string &bucket, const std::string &prefix,
                    uint32_t count, ListResultEntry *result,
                    bool *is_truncated, std::string *next_marker,
                    const std::string &marker) {
	int64_t ret = InitGlobalConfig();
  if (ret) {
		return ret;
	}

	SetResource(bucket, prefix, count, marker);

  std::string signature("");
  // 构建HTTP头部
  m_http->Reset();
  m_http->AddHeader("User-Agent", USERAGENT);
  m_http->AddHeader("Content-Length", std::to_string(0));
  std::map<std::string, std::string> params;
  params["prefix"] = prefix;
  params["marker"] = marker;
  params["max-keys"] = std::to_string(count);
  params["delimiter"] = delimiter_;
  SetURL(params);
  
  //使用 HTTP 信息构建签名
  UFileDigest digestor;
  ret = digestor.SignWithRequestV2(m_http, HEAD_FIELD_CHECK, bucket, "", "",
                                 signature, "listobjects", params);
  if (ret) {
    return ret;
  }
  m_http->AddHeader("Authorization", digestor.Token(signature));

  //设置输出
  std::ostringstream oss;
  UCloudOStream data_stream(&oss);
  UCloudHTTPWriteParam wp = {f : NULL, os : &data_stream};
  ret = m_http->RoundTrip(NULL, &wp, NULL);
  if (ret) {
    UFILE_SET_ERROR2(ERR_CPPSDK_SEND_HTTP, UFILE_LAST_ERRMSG());
    return ERR_CPPSDK_SEND_HTTP;
  }
  //解析回应
  long code = 200;
  ret = m_http->ResponseCode(&code);
  if (ret) {
    UFILE_SET_ERROR(ERR_CPPSDK_CURL);
    return ERR_CPPSDK_CURL;
  }

  std::string errmsg;
  if (code != 200) {
    int parse_ret = UFileErrorRsp(oss.str().c_str(), &ret, errmsg);
    if (parse_ret) {
      UFILE_SET_ERROR(ERR_CPPSDK_CLIENT_INTERNAL);
      return ERR_CPPSDK_CLIENT_INTERNAL;
    }
    UFILE_SET_ERROR2(ret, errmsg);
  } else {
    std::cout << oss.str().c_str();
  }
  return ret;
}

void UFileList::SetResource(const std::string &bucket,
                            const std::string &prefix,
                            uint32_t count,
                            const std::string &marker) {
  bucket_ = bucket;
  prefix_ = prefix;
  max_key_ = count > LIST_MAX_COUNT ? LIST_MAX_COUNT : count;
  next_marker_ = marker;
}

void UFileList::SetURL(std::map<std::string, std::string> params) {
  std::string url = UFileHost(bucket_);
  url = url + "/?listobjects";
  for (auto it = params.begin(); it != params.end(); ++it) {
    url = url + "&" + it->first + "=" + it->second;
  }
  m_http->SetURL(url);
}

} // namespace api
} // namespace cppsdk
} // namespace ucloud
