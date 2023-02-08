#pragma once

namespace authpp {

class CcidConnection;

class OathSession {
 public:
  OathSession(const CcidConnection& connection);

  void ListCredentials() const;
  void CalculateAll() const;

 private:
  const CcidConnection& connection;
};

}  // namespace authpp
