/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 09:19:48
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __012412__DEFINES_POLSKY_TV_H
   #define __012412__DEFINES_POLSKY_TV_H

#include <more_polsky_translations.h>

#define APP_NAME      "Polsky.TV"
#define UPD_CHECK_URL "http://rt.coujo.de/polsky_tv_ver.xml"
#define BIN_NAME      "polsky_tv"
#define API_SERVER    "iptv.polsky.tv"
#define COMPANY_NAME  "Polsky.TV"
#define COMPANY_LINK  "<a href='http://www.polsky.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define APPLICATION_SHORTCUT "pol"

#define SERVICE_CERTIFICATE  \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFfTCCBGWgAwIBAgIQQMVr9toAhxX3CfbbMQRIozANBgkqhkiG9w0BAQsFADCB\n" \
"kDELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\n" \
"A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxNjA0BgNV\n" \
"BAMTLUNPTU9ETyBSU0EgRG9tYWluIFZhbGlkYXRpb24gU2VjdXJlIFNlcnZlciBD\n" \
"QTAeFw0xNDEwMjUwMDAwMDBaFw0xNjEwMjQyMzU5NTlaMHsxITAfBgNVBAsTGERv\n" \
"bWFpbiBDb250cm9sIFZhbGlkYXRlZDEkMCIGA1UECxMbSG9zdGVkIGJ5IEFsZXhh\n" \
"bmRlciBCYXN1bm92MRQwEgYDVQQLEwtQb3NpdGl2ZVNTTDEaMBgGA1UEAxMRc2Vy\n" \
"dmljZS5wb2xza3kudHYwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCq\n" \
"U98qyLXNcudVtdTxC1Mc2lFqnKtgFY64RPLSoeRNG/d0i5XcrwTmBeXbeuwDelzY\n" \
"SeC6VNRQlW+uzB3gJUzSk+kvml1rlq+Mag0hWVPnz47ZeSTkAWtMKFJo9WvyRhDL\n" \
"P6vBrTH90hxMtTfT0KocB+kis/Meju6TUEwkHc24UQ+Ja36rFn0SdQoeyKfwDIhx\n" \
"c3jmtIMrfu6X4qfkNQNJCoGiytVf0U31QSTV7sRZfarQgpA5xXiGvCve9j58k8rg\n" \
"X++LUpf6UHlnZ4rw467cQNc0Ujk7tzWFNeT9v3VQyq8aMmu5Ks6YOfTaq24AOh07\n" \
"56TZ9bHFe2wqIxELKScbAgMBAAGjggHlMIIB4TAfBgNVHSMEGDAWgBSQr2o6lFoL\n" \
"2JDqElZz30O0Oija5zAdBgNVHQ4EFgQUkrH2vj7YcmducapdQ3ziGHWhf+4wDgYD\n" \
"VR0PAQH/BAQDAgWgMAwGA1UdEwEB/wQCMAAwHQYDVR0lBBYwFAYIKwYBBQUHAwEG\n" \
"CCsGAQUFBwMCME8GA1UdIARIMEYwOgYLKwYBBAGyMQECAgcwKzApBggrBgEFBQcC\n" \
"ARYdaHR0cHM6Ly9zZWN1cmUuY29tb2RvLmNvbS9DUFMwCAYGZ4EMAQIBMFQGA1Ud\n" \
"HwRNMEswSaBHoEWGQ2h0dHA6Ly9jcmwuY29tb2RvY2EuY29tL0NPTU9ET1JTQURv\n" \
"bWFpblZhbGlkYXRpb25TZWN1cmVTZXJ2ZXJDQS5jcmwwgYUGCCsGAQUFBwEBBHkw\n" \
"dzBPBggrBgEFBQcwAoZDaHR0cDovL2NydC5jb21vZG9jYS5jb20vQ09NT0RPUlNB\n" \
"RG9tYWluVmFsaWRhdGlvblNlY3VyZVNlcnZlckNBLmNydDAkBggrBgEFBQcwAYYY\n" \
"aHR0cDovL29jc3AuY29tb2RvY2EuY29tMDMGA1UdEQQsMCqCEXNlcnZpY2UucG9s\n" \
"c2t5LnR2ghV3d3cuc2VydmljZS5wb2xza3kudHYwDQYJKoZIhvcNAQELBQADggEB\n" \
"AFAgGiBD+d3N4qjP0jZvNiNURFzWVYsSIGgquLyjNiiEFt3yZ+N8Kcnwcp8xAaEs\n" \
"ey8sOu7XwneEA++el1gk8Duq+uwB2osFo+FScGoSJNPR+zcyl+J/8JNZjamqjJMx\n" \
"17v3dDbX8o1BGjuzmKdR+6u4dsS/Yns0QkjOZ2QyyADOO/EpgirWBYFPSrKljYpT\n" \
"irBB6H8v/P7AUA+b5M5dYo1ks+ReABfbOBHyhO85JFMtHI4eqan4jka9OGDr2xwG\n" \
"Sjn5st0Vkfil3FyBnJknIkgGZjos8hpy8OpbvudfACnFyVaz/a4X7kBElYLY7C6o\n" \
"DTWyrjtZwBK2HQxjXR3V4Uo=\n" \
"-----END CERTIFICATE-----\n"

#endif // __012412__DEFINES_POLSKY_TV_H
