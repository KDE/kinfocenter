
# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm-about-distro\")

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
