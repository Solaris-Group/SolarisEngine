# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/xpressive
    REF boost-${VERSION}
    SHA512 aab4b108085da3b216d4fa0f5117f517df4cef253a108a02dd27ebac53b8af5c61b76e2052506f6fcaf1b76e87d5cf9dab1d64b42f59a1a91a284706e9ea2356
    HEAD_REF master
)

set(FEATURE_OPTIONS "")
boost_configure_and_install(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${FEATURE_OPTIONS}
)