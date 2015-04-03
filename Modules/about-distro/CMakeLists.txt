
# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm-about-distro\")

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
