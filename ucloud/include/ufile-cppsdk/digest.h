#ifndef _UFILESDK_CPP_UCOUD_DIGEST_DIGEST_
#define _UFILESDK_CPP_UCOUD_DIGEST_DIGEST_

#include <string>
#include <map>
#include <ufile-cppsdk/http.h>

namespace ucloud {
namespace cppsdk {
namespace digest {

enum {
  HEAD_FIELD_CHECK = 1,
  QUERY_STRING_CHECK,
};

class UFileDigest {

public:
  UFileDigest();
  UFileDigest(const std::string &publicKey, const std::string &privateKey);
  ~UFileDigest();

  void SetKey(const std::string &publicKey, const std::string &privateKey);
  int SignWithData(const std::string &utf8String2Sign, std::string &signature);
  int SignWithRequest(
      ucloud::cppsdk::http::HTTPRequest *http, const int type,
      const std::string &bucket,
      const std::string &key
      //如果是 multipart 上传的话需要把文件内容的真实 mimetype 传递进来
      ,
      const std::string multipartMimetype, std::string &signature);
  int SignWithRequestV2(
      ucloud::cppsdk::http::HTTPRequest *http, const int type,
      const std::string &bucket,
      const std::string &key
      //如果是 multipart 上传的话需要把文件内容的真实 mimetype 传递进来
      ,
      const std::string multipartMimetype, std::string &signature,
      std::string action, std::map<std::string, std::string> params);

  std::string Token(const std::string &signature);

private:
  std::string CanonicalizedResource(const std::string &bucket,
                                    const std::string &key);
  std::string
  CanonicalizedUCloudHeaders(ucloud::cppsdk::http::HTTPRequest *http);
  std::string CanonicalizedURLParams(std::string action,
      std::map<std::string, std::string> params);

private:
  std::string m_ak;
  std::string m_sk;
};

} // namespace digest
} // namespace cppsdk
} // namespace ucloud

#endif
