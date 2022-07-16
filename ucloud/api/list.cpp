#include <ufile-cppsdk/common.h>
#include <ufile-cppsdk/config.h>
#include <ufile-cppsdk/list.h>
#include <ufile-cppsdk/digest.h>
#include <ufile-cppsdk/errno.h>
#include <ufile-cppsdk/http.h>
#include <ufile-cppsdk/urlcodec.h>

using namespace ucloud::cppsdk::http;
using namespace ucloud::cppsdk::utils;
using namespace ucloud::cppsdk::error;
using namespace ucloud::cppsdk::digest;
using namespace ucloud::cppsdk::config;

namespace ucloud {
namespace cppsdk {
namespace api {

UFileList::UFileList() : delimiter("/") {}

UFileList::~UFileList() {}

int UFileList::List(const std::string &bucket, const std::string &prefix,
                    uint32_t count, ListResultEntry *result,
                    bool *is_truncated, std::string *next_marker) {
	int64_t ret = InitGlobalConfig();
  if (ret) {
		return ret;
	}

	SetResource(bucket, key);

  std::string signature("");
  // 构建HTTP头部
  m_http->Reset();
  m_http->AddHeader("User-Agent", USERAGENT);
  m_http->SetURL(UFileHost(bucket) + EasyPathEscape(key));
  
  //使用 HTTP 信息构建签名
  UFileDigest digestor;
  ret = digestor.SignWithRequest(m_http, HEAD_FIELD_CHECK, bucket, key, "",
                                 signature);
  if (ret) {
    return ret;
  }
  m_http->AddHeader("Authorization", digestor.Token(signature));
}

void UFileDownload::SetResource(const std::string &bucket,
                                const std::string &key) {

  m_bucket = bucket;
  m_key = key;
}

} // namespace api
} // namespace cppsdk
} // namespace ucloud
