#pragma once

namespace authpp {

class CcidConnection;

class OathSession {
public:
    OathSession(const CcidConnection& connection);

private:
    const CcidConnection& connection;
};

} // namespace authpp
