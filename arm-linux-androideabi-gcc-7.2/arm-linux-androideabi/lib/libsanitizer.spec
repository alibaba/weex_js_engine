# This spec file is read by gcc when linking.  It is used to specify the
# standard libraries we need in order to link with various sanitizer libs.

*link_libasan: -ldl -ldl -lm

*link_libtsan: -ldl -ldl -lm

*link_libubsan: -ldl -ldl -lm

*link_liblsan: -ldl -ldl -lm

