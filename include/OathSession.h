#pragma once

namespace authpp {

class CcidConnection;

class OathSession {
public:
    OathSession(const CcidConnection& connection);

    void list_credentials() const;

private:
    const CcidConnection& connection;
};

} // namespace authpp
