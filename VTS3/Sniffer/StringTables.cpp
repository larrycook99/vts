//  ------ BACnet string tables---------------
//
// This file implements BACnetStringTable, a class that represents
// a mapping between an enumeration and a set of strings.

#include "stdafx.h"
#include "StringTables.h"

namespace NetworkSniffer {

// Return a text buffer for short term use, such as as an argument
// for sprintf.  Buffers are allocated from a small rotary set, so
// long-term usage will result in the buffer being re-used.
char* TempTextBuffer()
{
#define TEMP_TEST_N_BUFFERS 20
#define TEMP_TEST_BUFFERLENGTH 200
   static char buffers[TEMP_TEST_N_BUFFERS ][ TEMP_TEST_BUFFERLENGTH ];
   static int ix;

   ix = (ix + 1) % TEMP_TEST_N_BUFFERS;
   return buffers[ix];
}

// Constructor for non-extensible enumerations
BACnetStringTable::BACnetStringTable( const char* const *pStrings,
                                      const int         nStrings )
: m_pStrings(pStrings)
, m_nStrings(nStrings)
, m_nReserved(nStrings)
, m_nMax(nStrings)
{
}

// Constructor for enumerations defined by BACnet as extensible
BACnetStringTable::BACnetStringTable( const char* const *pStrings,
                                      const int         nStrings,
                                      const int         nReserved,
                                      const int         nMax )
: m_pStrings(pStrings)
, m_nStrings(nStrings)
, m_nReserved(nReserved)
, m_nMax(nMax)
{
}

// Return a string containing text for the specified enumerated value.
// If the value is undefined, the string will show the pUndefined title and the numeric value.
// If pUndefined is NULL, show "Reserved-XXX" or "Proprietary-XXX" depending on the value
const char* BACnetStringTable::EnumString( int theIndex, const char *pUndefined /* = NULL */ ) const
{
   const char *pRet;
   if (theIndex < m_nStrings)
   {
      pRet = m_pStrings[ theIndex ];
   }
   else
   {
      if (pUndefined == NULL)
      {
         // Vendor versus proprietary
         // 135.1 clause 4.4i says "proprietary" + anything_but_space + integer enumerations
         // 135.1 clause 4.4l says "proprietary" + space + integer for object IDs
         // But VTS historically used "Vendor"

         // "Proprietary" only if in an extensible range, else "Reserved"
         pUndefined = ((theIndex >= m_nReserved) && (theIndex < m_nMax))
                      ? "Proprietary-" : "Reserved-";
      }

      char *pTxt = TempTextBuffer();
      sprintf( pTxt, "%s%d", pUndefined, theIndex );
      pRet = pTxt;
   }

   return pRet;
}

// Return the enumeration value corresponding to pString.
// If the string is not a valid value, return -1.
// In addition to tabled strings, accepts
// "proprietary-anything-you-like-123" (per 135.1 clause 4.4i for enumerations)
// "proprietary 123" (per 135.1 clause 4.4l for object IDs)
// "vendor 123", "vendor-123", "reserved 123", reserved-123" matching older VTS usage
// "123" because is makes more sense than any of the above
int BACnetStringTable::EnumValue( const char *pString ) const
{
   int retval = -1;

   // Convert any underbars to dashes, matching our tables
   CString str(pString);
   str.Replace( '_', '-' );

   // Case-insensitive comparison
   for (int ix = 0; ix < m_nStrings; ix++)
   {
      if (str.CompareNoCase( m_pStrings[ ix ] ) == 0)
      {
         retval = ix;
         break;
      }
   }

   if (retval < 0)
   {
      // Not in the table.  Try for proprietary, vendor, reserved, or naked integer
      if ((_strnicmp(str, "proprietary", 11) == 0) ||
          (_strnicmp(str, "vendor", 6) == 0) ||
          (_strnicmp(str, "reserved", 8) == 0) ||
          (IsDigit(str[0])))
      {
         // Ignore any spaces or additional text and look for a final integer
         int firstDigit = str.FindOneOf( "0123456789" );
         if (firstDigit >= 0)
         {
            retval = atoi( (LPCTSTR)str + firstDigit );
         }
      }
   }

   return retval;
}

// Helpers to make a sorted list of strings
struct OurSorter
{
   UINT         m_ID;
   const char  *m_pString;
};

static int SortOurStuff( const void *pFirst, const void *pSecond )
{
   const OurSorter *pPS1 = (const OurSorter*)pFirst;
   const OurSorter *pPS2 = (const OurSorter*)pSecond;
   return strcmp( pPS1->m_pString, pPS2->m_pString );
}

// Fill a CComboBox with the contents of the string table
// If doSort is true, sort the strings alphabetically, else use string table order.
// CAUTION: if sorted, you must convert the selected string value to the enum,
// since the index in the combo won't be the string index for a sorted list.
void BACnetStringTable::FillCombo( CComboBox &theCombo, bool doSort ) const
{
   int ix;
   if (!doSort)
   {
      for (ix = 0; ix < m_nStrings; ix++)
      {
         theCombo.AddString( m_pStrings[ ix ] );
      }
   }
   else
   {
      // Sort the strings
      OurSorter *pStuff = new OurSorter[m_nStrings];
      for (ix = 0; ix < m_nStrings; ix++)
      {
         pStuff[ix].m_ID = ix;
         pStuff[ix].m_pString = m_pStrings[ix];
      }
      qsort( pStuff, m_nStrings, sizeof(OurSorter), &SortOurStuff );

      // Populate the combo
      for (ix = 0; ix < m_nStrings; ix++)
      {
         theCombo.AddString( pStuff[ix].m_pString );
      }

      delete[] pStuff;
   }
}

STRING_TABLE FalseTrue[] = {
   "False",
   "True"
};
BAC_STRINGTABLE(FalseTrue);

STRING_TABLE NoYes[] = {
   "No",
   "Yes"
};
BAC_STRINGTABLE(NoYes);

STRING_TABLE ApplicationTypes[] = {
   "Null",           // 0
   "Boolean",
   "Unsigned Integer",
   "Signed Integer",
   "Real",           // 4
   "Double",
   "Octet String",
   "Character String",
   "Bit String",     // 8
   "Enumerated",
   "Date",
   "Time",
   "BACnetObjectIdentifier" // 12
};
BAC_STRINGTABLE(ApplicationTypes);

// Not an explicit datatype: defined inline in BACnetAccumulatorRecord
STRING_TABLE BACnetAccumulatorStatus[] = {
   "normal",
   "starting",
   "recovered",
   "abnormal",
   "failed"
};
BAC_STRINGTABLE(BACnetAccumulatorStatus);

STRING_TABLE BACnetAction[] = {
   "direct",
   "reverse"
};
BAC_STRINGTABLE(BACnetAction);

STRING_TABLE BACnetBinaryPV[] = {
   "inactive",
   "active"
};
BAC_STRINGTABLE(BACnetBinaryPV);

// As BACnetBinaryPV, but adding a NULL option
STRING_TABLE BACnetBinaryPV_orNull[] = {
   "inactive",
   "active",
   "null"
};
BAC_STRINGTABLE(BACnetBinaryPV_orNull);

STRING_TABLE BACnetDeviceStatus[] = {
   "operational",
   "operational-read-only",
   "download-required",
   "download-in-progress",
   "non-operational",
   "backup-in-progress"
};
BAC_STRINGTABLE_EX(BACnetDeviceStatus, 64, 65536);

STRING_TABLE BACnetDoorAlarmState[] = {
   "normal",
   "alarm",
   "door-open-too-long",
   "forced-open",
   "tamper",
   "door-fault",
   "lock-down",
   "free-access",
   "egress-open"
};
BAC_STRINGTABLE_EX(BACnetDoorAlarmState, 256, 65536);

STRING_TABLE BACnetDoorSecuredStatus[] = {
   "secured",
   "unsecured",
   "unknown",
};
BAC_STRINGTABLE(BACnetDoorSecuredStatus);

STRING_TABLE BACnetDoorStatus[] = {
   "closed",
   "open",
   "unknown",
};
BAC_STRINGTABLE(BACnetDoorStatus);

STRING_TABLE BACnetDoorValue[] = {
   "lock",
   "unlock",
   "pulse-unlock",
   "extended-pulse-unlock",
};
BAC_STRINGTABLE(BACnetDoorValue);

STRING_TABLE BACnetDoorValue_orNull[] = {
   "lock",
   "unlock",
   "pulse-unlock",
   "extended-pulse-unlock",
   "null"
};
BAC_STRINGTABLE(BACnetDoorValue_orNull);

// These strings are copied from Clause 21, and match
// the capitalization and punctuation used there.
STRING_TABLE BACnetEngineeringUnits[] = {
   "square-meters",                    // 0
   "square-feet",                      // 1
   "milliamperes",                     // 2
   "amperes",                          // 3
   "ohms",                             // 4
   "volts",                            // 5
   "kilovolts",                        // 6
   "megavolts",                        // 7
   "volt-amperes",                     // 8
   "kilovolt-amperes",                 // 9
   "megavolt-amperes",                 // 10
   "volt-amperes-reactive",            // 11
   "kilovolt-amperes-reactive",        // 12
   "megavolt-amperes-reactive",        // 13
   "degrees-phase",                    // 14
   "power-factor",                     // 15
   "joules",                           // 16
   "kilojoules",                       // 17
   "watt-hours",                       // 18
   "kilowatt-hours",                   // 19
   "btus",                             // 20
   "therms",                           // 21
   "ton-hours",                        // 22
   "joules-per-kilogram-dry-air",      // 23
   "btus-per-pound-dry-air",           // 24
   "cycles-per-hour",                  // 25
   "cycles-per-minute",                // 26
   "hertz",                            // 27
   "grams-of-water-per-kilogram-dry-air", // 28
   "percent-relative-humidity",        // 29
   "millimeters",                      // 30
   "meters",                           // 31
   "inches",                           // 32
   "feet",                             // 33
   "watts-per-square-foot",            // 34
   "watts-per-square-meter",           // 35
   "lumens",                           // 36
   "luxes",                            // 37
   "foot-candles",                     // 38
   "kilograms",                        // 39
   "pounds-mass",                      // 40
   "tons",                             // 41
   "kilograms-per-second",             // 42
   "kilograms-per-minute",             // 43
   "kilograms-per-hour",               // 44
   "pounds-mass-per-minute",           // 45
   "pounds-mass-per-hour",             // 46
   "watts",                            // 47
   "kilowatts",                        // 48
   "megawatts",                        // 49
   "btus-per-hour",                    // 50
   "horsepower",                       // 51
   "tons-refrigeration",               // 52
   "pascals",                          // 53
   "kilopascals",                      // 54
   "bars",                             // 55
   "pounds-force-per-square-inch",     // 56
   "centimeters-of-water",             // 57
   "inches-of-water",                  // 58
   "millimeters-of-mercury",           // 59
   "centimeters-of-mercury",           // 60
   "inches-of-mercury",                // 61
   "degrees-Celsius",                  // 62
   "degrees-Kelvin",                   // 63
   "degrees-Fahrenheit",               // 64
   "degree-days-Celsius",              // 65
   "degree-days-Fahrenheit",           // 66
   "years",                            // 67
   "months",                           // 68
   "weeks",                            // 69
   "days",                             // 70
   "hours",                            // 71
   "minutes",                          // 72
   "seconds",                          // 73
   "meters-per-second",                // 74
   "kilometers-per-hour",              // 75
   "feet-per-second",                  // 76
   "feet-per-minute",                  // 77
   "miles-per-hour",                   // 78
   "cubic-feet",                       // 79
   "cubic-meters",                     // 80
   "imperial-gallons",                 // 81
   "liters",                           // 82
   "us-gallons",                       // 83
   "cubic-feet-per-minute",            // 84
   "cubic-meters-per-second",          // 85
   "imperial-gallons-per-minute",      // 86
   "liters-per-second",                // 87
   "liters-per-minute",                // 88
   "us-gallons-per-minute",            // 89
   "degrees-angular",                  // 90
   "degrees-Celsius-per-hour",         // 91
   "degrees-Celsius-per-minute",       // 92
   "degrees-Fahrenheit-per-hour",      // 93
   "degrees-Fahrenheit-per-minute",    // 94
   "no-units",                         // 95
   "parts-per-million",                // 96
   "parts-per-billion",                // 97
   "percent",                          // 98
   "percent-per-second",               // 99
   "per-minute",                       // 100
   "per-second",                       // 101
   "psi-per-degree-Fahrenheit",        // 102
   "radians",                          // 103
   "revolutions-per-minute",           // 104

   /* added in second or third public review */
   "currency1",                        // 105
   "currency2",                        // 106
   "currency3",                        // 107
   "currency4",                        // 108
   "currency5",                        // 109
   "currency6",                        // 110
   "currency7",                        // 111
   "currency8",                        // 112
   "currency9",                        // 113
   "currency10",                       // 114
   "square-inches",                    // 115
   "square-centimeters",               // 116
   "btus-per-pound",                   // 117
   "centimeters",                      // 118
   "pounds-mass-per-second",           // 119
   "delta-degrees-Fahrenheit",         // 120
   "delta-degrees-Kelvin",             // 121
   "kilohms",                          // 122
   "megohms",                          // 123
   "millivolts",                       // 124
   "kilojoules-per-kilogram",          // 125
   "megajoules",                       // 126
   "joules-per-degree-Kelvin",         // 127
   "joules-per-kilogram-degree-Kelvin",// 128
   "kilohertz",                        // 129
   "megahertz",                        // 130
   "per-hour",                         // 131
   "milliwatts",                       // 132
   "hectopascals",                     // 133
   "millibars",                        // 134
   "cubic-meters-per-hour",            // 135
   "liters-per-hour",                  // 136
   "kilowatt-hours-per-square-meter",  // 137
   "kilowatt-hours-per-square-foot",   // 138
   "megajoules-per-square-meter",      // 139
   "megajoules-per-square-foot",       // 140
   "watts-per-square-meter-degree-kelvin", // 141

   // New units added 3/9/2008
   "cubic-feet-per-second",            // 142
   "percent-obscuration-per-foot",     // 143
   "percent-obscuration-per-meter",    // 144
   "milliohms",                        // 145
   "megawatt-hours",                   // 146
   "kilo-btus",                        // 147
   "mega-btus",                        // 148
   "kilojoules-per-kilogram-dry-air",  // 149
   "megajoules-per-kilogram-dry-air",  // 150
   "kilojoules-per-degree-Kelvin",     // 151
   "megajoules-per-degree-Kelvin",     // 152
   "newton",                           // 153
   "grams-per-second",                 // 154
   "grams-per-minute",                 // 155
   "tons-per-hour",                    // 156
   "kilo-btus-per-hour",               // 157
   "hundredths-seconds",               // 158
   "milliseconds",                     // 159
   "newton-meters",                    // 160
   "millimeters-per-second",           // 161
   "millimeters-per-minute",           // 162
   "meters-per-minute",                // 163
   "meters-per-hour",                  // 164
   "cubic-meters-per-minute",          // 165
   "meters-per-second-per-second",     // 166
   "amperes-per-meter",                // 167
   "amperes-per-square-meter",         // 168
   "ampere-square-meters",             // 169
   "farads",                           // 170
   "henrys",                           // 171
   "ohm-meters",                       // 172
   "siemens",                          // 173
   "siemens-per-meter",                // 174
   "teslas",                           // 175
   "volts-per-degree-Kelvin",          // 176
   "volts-per-meter",                  // 177
   "webers",                           // 178
   "candelas",                         // 179
   "candelas-per-square-meter",        // 180
   "degrees-Kelvin-per-hour",          // 181
   "degrees-Kelvin-per-minute",        // 182
   "joule-seconds",                    // 183
   "radians-per-second",               // 184
   "square-meters-per-Newton",         // 185
   "kilograms-per-cubic-meter",        // 186
   "newton-seconds",                   // 187
   "newtons-per-meter",                // 188
   "watts-per-meter-per-degree-Kelvin",// 189 last definition in 135-2008

   "micro-siemens",                    // 190
   "cubic-feet-per-hour",              // 191
   "us-gallons-per-hour",              // 192
   "kilometers",                       // 193
   "micrometers",                      // 194
   "grams",                            // 195
   "milligrams",                       // 196
   "milliliters",                      // 197
   "milliliters-per-second",           // 198
   "decibels",                         // 199
   "decibels-millivolt",               // 200
   "decibels-volt",                    // 201
   "millisiemens",                     // 202
   "watt-hours-reactive",              // 203
   "kilowatt-hours-reactive",          // 204
   "megawatt-hours-reactive",          // 205
   "millimeters-of-water",             // 206
   "per-mille",                        // 207
   "grams-per-gram",                   // 208
   "kilograms-per-kilogram",           // 209
   "grams-per-kilogram",               // 210
   "milligrams-per-gram",              // 211
   "milligrams-per-kilogram",          // 212
   "grams-per-milliliter",             // 213
   "grams-per-liter",                  // 214
   "milligrams-per-liter",             // 215
   "micrograms-per-liter",             // 216
   "grams-per-cubic-meter",            // 217
   "milligrams-per-cubic-meter",       // 218
   "micrograms-per-cubic-meter",       // 219
   "nanograms-per-cubic-meter",        // 220
   "grams-per-cubic-centimeter",       // 221
   "becquerels",                       // 222
   "kilobecquerels",                   // 223
   "megabecquerels",                   // 224
   "gray",                             // 225
   "milligray",                        // 226
   "microgray",                        // 227
   "sieverts",                         // 228
   "millisieverts",                    // 229
   "microsieverts",                    // 230
   "microsieverts-per-hour",           // 231
   "decibels-a",                       // 232
   "nephelometric-turbidity-unit",     // 233
   "pH",                               // 234
   "grams-per-square-meter",           // 235
   "minutes-per-degree-kelvin",        // 236 last definition in 135-2012
};
BAC_STRINGTABLE_EX(BACnetEngineeringUnits, 256, 65536);

STRING_TABLE BACnetError[] = {
/* Alarm and Event Services */
   "AcknowledgeAlarm Error Choice",
   "ConfirmedCOVNotification Error Choice",
   "ConfirmedEventNotification Error Choice",
   "GetAlarmSummary Error Choice",
   "GetEnrollmentSummary Error Choice",
   "SubscribeCOV Error Choice",           // [5]

/* File Access Services */
   "AtomicReadFile Error Choice",
   "AtomicWriteFile Error Choice",

/* Object Access Services */
   "AddListElement Error Choice",
   "RemoveListElement Error Choice",
   "CreateObject Error Choice",           // [10]
   "DeleteObject Error Choice",
   "ReadProperty Error Choice",
   "ReadPropertyConditional Error Choice",
   "ReadPropertyMultiple Error Choice",
   "WriteProperty Error Choice",          // [15]
   "WritePropertyMultiple Error Choice",

/* Remote Device Management Services */
   "DeviceCommunicationControl Error Choice",
   "ConfirmedPrivateTransfer Error Choice",
   "ConfirmedTextMessage Error Choice",
   "ReinitializeDevice Error Choice",     // [20]

/* Virtual Terminal Services */
   "VT-Open Error Choice",
   "VT-Close Error Choice",
   "VT-Data Error Choice",

/* Security Services */
   "Authenticate Error Choice",
   "RequestKey Error Choice",             // [25]

/* services added after 1995 */
   "ReadRange Error Choice",
   "Life Safety Operation Error Choice",
   "SubscribeCOVProperty Error Choice",
   "GetEventInformation Error Choice",    // [29] last definition in 135-2012
};
BAC_STRINGTABLE(BACnetError);

STRING_TABLE BACnetErrorClass[] = {
   "device",
   "object",
   "property",
   "resources",
   "security",
   "services",
   "vt",
   "communication",    // 7 last definition in 135-2012
};
BAC_STRINGTABLE_EX(BACnetErrorClass, 64, 65536);

STRING_TABLE BACnetErrorCode[] = {
   "other",                              /* 0 */
   "authentication-failed",              /* 1 */
   "configuration-in-progress",          /* 2 */
   "device-busy",                        /* 3 */
   "dynamic-creation-not-supported",     /* 4 */
   "file-access-denied",                 /* 5 */
   "incompatible-security-levels",       /* 6 */
   "inconsistent-parameters",            /* 7 */
   "inconsistent-selection-criterion",   /* 8 */
   "invalid-data-type",                  /* 9 */
   "invalid-file-access-method",         /* 10 */
   "invalid-file-start-position",        /* 11 */
   "invalid-operator-name",              /* 12 */
   "invalid-parameter-data-type",        /* 13 */
   "invalid-time-stamp",                 /* 14 */
   "key-generation-error",               /* 15 */
   "missing-required-parameter",         /* 16 */
   "no-objects-of-specified-type",       /* 17 */
   "no-space-for-object",                /* 18 */
   "no-space-to-add-list-element",       /* 19 */
   "no-space-to-write-property",         /* 20 */
   "no-vt-sessions-available",           /* 21 */
   "property-is-not-a-list",             /* 22 */
   "object-deletion-not-permitted",      /* 23 */
   "object-identifier-already-exists",   /* 24 */
   "operational-problem",                /* 25 */
   "password-failure",                   /* 26 */
   "read-access-denied",                 /* 27 */
   "security-not-supported",             /* 28 */
   "service-request-denied",             /* 29 */
   "timeout",                            /* 30 */
   "unknown-object",                     /* 31 */
   "unknown-property",                   /* 32 */
   "invalid enumeration",                /* 33 */
   "unknown-vt-class",                   /* 34 */
   "unknown-vt-session",                 /* 35 */
   "unsupported-object-type",            /* 36 */
   "value-out-of-range",                 /* 37 */
   "vt-session-already-closed",          /* 38 */
   "vt-session-termination-failure",     /* 39 */
   "write-access-denied",                /* 40 */
   "character-set-not-supported",        /* 41 */
   "invalid-array-index",                /* 42 */
   "cov-subscription-failed",            /* 43 kare.sars@wapice.com */
   "not-cov-property",                   /* 44 | */
   "optional-functionality-not-supported",/*45 | */
   "invalid-configuration-data",         /* 46 | */
   "datatype-not-supported",             /* 47 | */
   "duplicate-name",                     /* 48 | */
   "duplicate-object-id",                /* 49 | */
   "property-is-not-an-array",           /* 50 kare.sars@wapice.com */
   // Added by Addenda B PPR3 (135-2004)
   "abort-buffer-overflow",              /* 51 */
   "abort-invalid-apdu-in-this-state",
   "abort-preempted-by-higher-priority-task",
   "abort-segmentation-not-supported",
   "abort-proprietary",
   "abort-other",                        /* 56 */
   "invalid-tag",
   "network-down",
   "reject-buffer-overflow",
   "reject-inconsistent-parameters",
   "reject-invalid-parameter-data-type",
   "reject-invalid-tag",                 /* 62 */
   "reject-missing-required-parameter",
   "reject-parameter-out-of-range",
   "reject-too-many-arguments",
   "reject-undefined-enumeration",
   "reject-unrecognized-service",        /* 67 */
   "reject-proprietary",                 /* 68 */
   "reject-other",
   "unknown-device",
   "unknown-route",
   "value-not-totalized",                /* 72 */
   // Added by Addenda D (135-2004)
   "invalid-event-state",                /* 73 */
   "no-alarm-configured",                /* 74 */
   // added by addenda b
   "log-buffer-full",                    // 75
   "logged-value-purged",                // 76
   "no-property-specified",              // 77
   "not-configured-for-triggered-logging", // 78
   // added by Addenda H (135-2004)
   "unknown-subscription",              // 79

   "parameter-out-of-range",           // 80
   "list-element-not-found",           // 81
   "busy",                             // 82
   "communication-disabled",           // 83
   "success",                          // 84
   "access-denied",                    // 85
   "bad-destination-address",          // 86
   "bad-destination-device-id",        // 87
   "bad-signature",                    // 88
   "bad-source-address",               // 89
   "bad-timestamp",                    // 90
   "cannot-use-key",                   // 91
   "cannot-verify-message-id",         // 92
   "correct-key-revision",             // 93
   "destination-device-id-required",   // 94
   "duplicate-message",                // 95
   "encryption-not-configured",        // 96
   "encryption-required",              // 97
   "incorrect-key",                    // 98
   "invalid-key-data",                 // 99
   "key-update-in-progress",           // 100
   "malformed-message",                // 101
   "not-key-server",                   // 102
   "security-not-configured",          // 103
   "source-security-required",         // 104
   "too-many-keys",                    // 105
   "unknown-authentication-type",      // 106
   "unknown-key",                      // 107
   "unknown-key-revision",             // 108
   "unknown-source-message",           // 109
   "not-router-to-dnet",               // 110
   "router-busy",                      // 111
   "unknown-network-message",          // 112
   "message-too-long",                 // 113
   "security-error",                   // 114
   "addressing-error",                 // 115
   "write-bdt-failed",                 // 116
   "read-bdt-failed",                  // 117
   "register-foreign-device-failed",   // 118
   "read-fdt-failed",                  // 119
   "delete-fdt-entry-failed",          // 120
   "distribute-broadcast-failed",      // 121
   "unknown-file-size",                // 122
   "abort-apdu-too-long",              // 123
   "abort-application-exceeded-reply-time", // 124
   "abort-out-of-resources",           // 125
   "abort-tsm-timeout",                // 126
   "abort-window-size-out-of-range",   // 127
   "file-full",                        // 128
   "inconsistent-configuration",       // 129
   "inconsistent-object-type",         // 130
   "internal-error",                   // 131
   "not-configured",                   // 132
   "out-of-memory",                    // 133
   "value-too-long",                   // 134
   "abort-insufficient-security",      // 135
   "abort-security-error",             // 136 Last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetErrorCode, 256, 65536);

STRING_TABLE BACnetEventState[] = {
   "normal",
   "fault",
   "offnormal",
   "high-limit",
   "low-limit",
   "life-safety-alarm",
};
BAC_STRINGTABLE_EX(BACnetEventState, 64, 65536);

STRING_TABLE BACnetEventTransitionBits[] = {
   "to-offnormal",
   "to-fault",
   "to-normal",
};
BAC_STRINGTABLE(BACnetEventTransitionBits);

STRING_TABLE BACnetEventType[] = {
   "change-of-bitstring",     // 0
   "change-of-state",
   "change-of-value",
   "command-failure",
   "floating-limit",
   "out-of-range",            // 5
   "complex-event-type",
   "deprecated",
   "change-of-life-safety",
   "extended",
   "buffer-ready",            // 10
   "unsigned-range",          // 11 last in 135-2008
   "reserved",                // 12
   "access-event",            // 13
   "double-out-of-range",     // 14
   "signed-out-of-range",     // 15
   "unsigned-out-of-range",   // 16
   "change-of-characterstring",// 17
   "change-of-status-flags",  // 18
   "change-of-reliability",   // 19
   "none",                    // 20 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetEventType, 64, 65536);

STRING_TABLE Acknowledgement_Filter[] = {
   "all",
   "acked",
   "not-acked"
};
BAC_STRINGTABLE(Acknowledgement_Filter);

STRING_TABLE EventState_Filter[] = {
   "offnormal",
   "fault",
   "normal",
   "all",
   "active"
};
BAC_STRINGTABLE(EventState_Filter);

STRING_TABLE BACnetFaultType[] = {
   "none",
   "fault-characterstring",
   "fault-extended",
   "fault-life-safety",
   "fault-state",
   "fault-status-flags"
};
BAC_STRINGTABLE(BACnetFaultType);

STRING_TABLE BACnetFileAccessMethod[] = {
   "record-access",
   "stream-access"
};
BAC_STRINGTABLE(BACnetFileAccessMethod);

STRING_TABLE BACnetLifeSafetyMode[] = {
   "off",               // 0
   "on",
   "test",
   "manned",
   "unmanned",
   "armed",             // 5
   "disarmed",
   "prearmed",
   "slow",
   "fast",
   "disconnected",      // 10
   "enabled",
   "disabled",
   "automatic-release-disabled",
   "default"            // 14 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetLifeSafetyMode, 256, 65536);

STRING_TABLE BACnetLifeSafetyOperation[] = {
   "none",              // 0
   "silence",
   "silence-audible",
   "silence-visual",
   "reset",
   "reset-alarm",       // 5
   "reset-fault",
   "unsilence",
   "unsilence-audible",
   "unsilence-visual",  // 9 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetLifeSafetyOperation, 64, 65536);

STRING_TABLE BACnetLifeSafetyState[] = {
   "quiet",             // 0
   "pre-alarm",
   "alarm",
   "fault",
   "fault-pre-alarm",
   "fault-alarm",
   "not-ready",
   "active",
   "tamper",
   "test-alarm",
   "test-active",       // 10
   "test-fault",
   "test-fault-alarm",
   "holdup",
   "duress",
   "tamper-alarm",
   "abnormal",
   "emergency-power",
   "delayed",
   "blocked",
   "local-alarm",       // 20
   "general-alarm",
   "supervisory",
   "test-supervisory"   // 23 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetLifeSafetyState, 256, 65536);

STRING_TABLE BACnetLightingTransition[] = {
   "none",
   "fade",
   "ramp"
};
BAC_STRINGTABLE_EX(BACnetLightingTransition, 64, 256);

STRING_TABLE BACnetLimitEnable[] = {
   "lowLimitEnable",
   "highLimitEnable"
};
BAC_STRINGTABLE(BACnetLimitEnable);

STRING_TABLE BACnetLockStatus[] = {
   "locked",
   "unlocked",
   "fault",
   "unknown",
};
BAC_STRINGTABLE(BACnetLockStatus);

STRING_TABLE BACnetLoggingType[] = {
   "polled",
   "cov",
   "triggered",
};
BAC_STRINGTABLE(BACnetLoggingType);

STRING_TABLE BACnetLogStatus[] = {
   "log-disabled",
   "buffer-purged",
   "log-interrupted",
};
BAC_STRINGTABLE_EX(BACnetLogStatus, 64, 256);

STRING_TABLE  BACnetMaintenance[] = {
   "none",
   "periodic-test",
   "need-service-operational",
   "need-service-inoperative"
};
BAC_STRINGTABLE_EX(BACnetMaintenance, 256, 65536);

STRING_TABLE BACnetNodeType[] = {
   "unknown",
   "system",
   "network",
   "device",
   "organization",
   "area",
   "equipment",
   "point",
   "collection",
   "property",
   "functional",
   "other"
};
BAC_STRINGTABLE(BACnetNodeType);
   
STRING_TABLE BACnetNotifyType[] = {
   "alarm",
   "event",
   "ack-notification"
};
BAC_STRINGTABLE(BACnetNotifyType);

// This list is also used to create the hash table for scripting.
STRING_TABLE BACnetObjectType[] = {
   "analog-input",          /* 0 */
   "analog-output",         /* 1 */
   "analog-value",          /* 2 */
   "binary-input",          /* 3 */
   "binary-output",         /* 4 */
   "binary-value",          /* 5 */
   "calendar",              /* 6 */
   "command",               /* 7 */
   "device",                /* 8 */
   "event-enrollment",      /* 9 */
   "file",                  /* 10 */
   "group",                 /* 11 */
   "loop",                  /* 12 */
   "multi-state-input",     /* 13 */
   "multi-state-output",    /* 14 */
   "notification-class",    /* 15 */
   "program",               /* 16 */
   "schedule",              /* 17 */
   "averaging",             /* 18 */
   "multi-state-value",     /* 19 */
   "trend-log" ,            /* 20 */
   "life-safety-point",     /* 21 */
   "life-safety-zone",      /* 22 */
   "accumulator",           // 23
   "pulse-converter",       // 24
   "event-log",             // 25 - Addendum B
   "global-group",          // 26 - Addendum B
   "trend-log-multiple",    // 27 - Addendum B
   "load-control",          // 28 - Addendum E 135-2004
   "structured-view",       // 29 - Addendum D
   "access-door",           // 30 Last in 135-2008.  Protocol revision 7
   "objtype-31",            // 31 This was lighting-out during an early review, but unused in 135-2012
   "access-credential",     // 32
   "access-point",          // 33
   "access-rights",         // 34
   "access-user",           // 35
   "access-zone",           // 36
   "credential-data-input", // 37
   "network-security",      // 38 Addendum 2008-g
   "bitstring-value",       // 39 Addendum 2008-w
   "characterstring-value", // 40
   "date-pattern-value",    // 41
   "date-value",            // 42
   "datetime-pattern-value",// 43
   "datetime-value",        // 44
   "integer-value",         // 45
   "large-analog-value",    // 46
   "octetstring-value",     // 47
   "positive-integer-value",// 48
   "time-pattern-value",    // 49
   "time-value",            // 50 Last in 2008-w
   "notification-forwarder",// 51 135-2010-af.  Protocol revision 13
   "alert-enrollment",      // 52 135-2010-af.  Protocol revision 13
   "channel",               // 53 135-2010-aa.  Protocol revision 14
   "lighting-output"        // 54 Last  in 135-2012.  Protocol revision 14

   // TODO: if you add an object type here, you must also
   // - Add the string to StandardObjects in Vtsapi32.cpp (which is capitalized and uses spaces instead of hyphens)
   // - Add a string to the enumeration BACnetObjectType in VTS.h (which will change MAX_DEFINED_OBJ)
   // - Add a case to GetObjectTable in DUDTOOL.CPP, and a table of supported properties for the object
   // - Add a table in stdobjpr.h for the object's properties
   // - For each type, there is also an icon for use in the EpicsTree view,
   //   IDB_EPICSTREE  BITMAP "res\\epicstree.bmp"
   //   So you may need to do some artwork there. (Some extra blank bitmaps have been added, but you can decorate them)
   // - Update other code, sprinkled throughout the universe, especially in DUDTOOL
};
BAC_STRINGTABLE_EX(BACnetObjectType, 128, 1024);

STRING_TABLE BACnetPolarity[] = {
   "normal",
   "reverse"
};
BAC_STRINGTABLE(BACnetPolarity);

STRING_TABLE BACnetProgramError[] = {
   "normal",
   "load-failed",
   "internal",
   "program",
   "other"
};
BAC_STRINGTABLE_EX(BACnetProgramError, 64, 65536);

STRING_TABLE BACnetProgramRequest[] = {
   "ready",
   "load",
   "run",
   "halt",
   "restart",
   "unload"
};
BAC_STRINGTABLE(BACnetProgramRequest);

STRING_TABLE BACnetProgramState[] = {
   "idle",
   "loading",
   "running",
   "waiting",
   "halted",
   "unloading"
};   
BAC_STRINGTABLE(BACnetProgramState);

// This list is also used to create the hash table for scripting.
STRING_TABLE BACnetPropertyIdentifier[] = {
   "acked-transitions",                /* 0 */
   "ack-required",                     /* 1 */
   "action",                           /* 2 */
   "action-text",                      /* 3 */
   "active-text",                      /* 4 */
   "active-vt-sessions",               /* 5 */
   "alarm-value",                      /* 6 */
   "alarm-values",                     /* 7 */
   "all",                              /* 8 */
   "all-writes-successful",            /* 9 */
   "apdu-segment-timeout",             /* 10 */
   "apdu-timeout",                     /* 11 */
   "application-software-version",     /* 12 */
   "archive",                          /* 13 */
   "bias",                             /* 14 */
   "change-of-state-count",            /* 15 */
   "change-of-state-time",             /* 16 */
   "notification-class",               /* 17 renamed from "class" in 2nd public review */
   "unused-propertyid-18",             /* 18 */
   "controlled-variable-reference",    /* 19 */
   "controlled-variable-units",        /* 20 */
   "controlled-variable-value",        /* 21 */
   "cov-increment",                    /* 22 */
   "date-list",                        /* 23 */
   "daylight-savings-status",          /* 24 */
   "deadband",                         /* 25 */
   "derivative-constant",              /* 26 */
   "derivative-constant-units",        /* 27 */
   "description",                      /* 28 */
   "description-of-halt",              /* 29 */
   "device-address-binding",           /* 30 */
   "device-type",                      /* 31 */
   "effective-period",                 /* 32 */
   "elapsed-active-time",              /* 33 */
   "error-limit",                      /* 34 */
   "event-enable",                     /* 35 */
   "event-state",                      /* 36 */
   "event-type",                       /* 37 */
   "exception-schedule",               /* 38 */
   "fault-values",                     /* 39 */
   "feedback-value",                   /* 40 */
   "file-access-method",               /* 41 */
   "file-size",                        /* 42 */
   "file-type",                        /* 43 */
   "firmware-revision",                /* 44 */
   "high-limit",                       /* 45 */
   "inactive-text",                    /* 46 */
   "in-process",                       /* 47 */
   "instance-of",                      /* 48 */
   "integral-constant",                /* 49 */
   "integral-constant-units",          /* 50 */
   "unused-was-issue-confirmed-notifications", /* 51 deleted in version 1 revision 4 */
   "limit-enable",                     /* 52 */
   "list-of-group-members",            /* 53 */
   "list-of-object-property-references", /* 54 */
   "list-of-session-keys",             /* 55 deleted in version 1 revision 11 */
   "local-date",                       /* 56 */
   "local-time",                       /* 57 */
   "location",                         /* 58 */
   "low-limit",                        /* 59 */
   "manipulated-variable-reference",   /* 60 */
   "maximum-output",                   /* 61 */
   "max-apdu-length-accepted",         /* 62 */
   "max-info-frames",                  /* 63 */
   "max-master",                       /* 64 */
   "max-pres-value",                   /* 65 */
   "minimum-off-time",                 /* 66 */
   "minimum-on-time",                  /* 67 */
   "minimum-output",                   /* 68 */
   "min-pres-value",                   /* 69 */
   "model-name",                       /* 70 */
   "modification-date",                /* 71 */
   "notify-type",                      /* 72 */
   "number-of-apdu-retries",           /* 73 */
   "number-of-states",                 /* 74 */
   "object-identifier",                /* 75 */
   "object-list",                      /* 76 */
   "object-name",                      /* 77 */
   "object-property-reference",        /* 78 */
   "object-type",                      /* 79 */
   "optional",                         /* 80 */
   "out-of-service",                   /* 81 */
   "output-units",                     /* 82 */
   "event-parameters",                 /* 83 */
   "polarity",                         /* 84 */
   "present-value",                    /* 85 */
   "priority",                         /* 86 */
   "priority-array",                   /* 87 */
   "priority-for-writing",             /* 88 */
   "process-identifier",               /* 89 */
   "program-change",                   /* 90 */
   "program-location",                 /* 91 */
   "program-state",                    /* 92 */
   "proportional-constant",            /* 93 */
   "proportional-constant-units",      /* 94 */
   "was-protocol-conformance-class",   /* 95 removed in version 1 rev 2 */
   "protocol-object-types-supported",  /* 96 */
   "protocol-services-supported",      /* 97 */
   "protocol-version",                 /* 98 */
   "read-only",                        /* 99 */
   "reason-for-halt",                  /* 100 */
   "was-recipient",                    /* 101 removed in version 1 rev 4 */
   "recipient-list",                   /* 102 */
   "reliability",                      /* 103 */
   "relinquish-default",               /* 104 */
   "required",                         /* 105 */
   "resolution",                       /* 106 */
   "segmentation-supported",           /* 107 */
   "setpoint",                         /* 108 */
   "setpoint-reference",               /* 109 */
   "state-text",                       /* 110 */
   "status-flags",                     /* 111 */
   "system-status",                    /* 112 */
   "time-delay",                       /* 113 */
   "time-of-active-time-reset",        /* 114 */
   "time-of-state-count-reset",        /* 115 */
   "time-synchronization-recipients",  /* 116 */
   "units",                            /* 117 */
   "update-interval",                  /* 118 */
   "utc-offset",                       /* 119 */
   "vendor-identifier",                /* 120 */
   "vendor-name",                      /* 121 */
   "vt-classes-supported",             /* 122 */
   "weekly-schedule",                  /* 123 */
   "attempted-samples",                /* 124 */
   "average-value",                    /* 125 */
   "buffer-size",                      /* 126 */
   "client-cov-increment",             /* 127 */
   "cov-resubscription-interval",      /* 128 */
   "was-current-notify-time",          /* 129 deleded in version 1 rev 3 */
   "event-time-stamps",                /* 130 */
   "log-buffer",                       /* 131 */
   "log-device-object-property",       /* 132 */
   "enable",                           /* 133 changed from log-enable in 135-2004b-5 */
   "log-interval",                     /* 134 */
   "maximum-value",                    /* 135 */
   "minimum-value",                    /* 136 */
   "notification-threshold",           /* 137 */
   "was-previous-notify-time",         /* 138 deleted in version 1 rev 3 */
   "protocol-revision",                /* 139 */
   "records-since-notification",       /* 140 */
   "record-count",                     /* 141 */
   "start-time",                       /* 142 */
   "stop-time",                        /* 143 */
   "stop-when-full",                   /* 144 */
   "total-record-count",               /* 145 */
   "valid-samples",                    /* 146 */
   "window-interval",                  /* 147 */
   "window-samples",                   /* 148 */
   "maximum-value-timestamp",          /* 149 */
   "minimum-value-timestamp",          /* 150 */
   "variance-value",                   /* 151 */
   "active-cov-subscriptions",         /* 152 */
   "backup-failure-timeout",           /* 153 */
   "configuration-files",              /* 154 */
   "database-revision",                /* 155 */
   "direct-reading",                   /* 156 */
   "last-restore-time",                /* 157 */
   "maintenance-required",             /* 158 */
   "member-of",                        /* 159 */
   "mode",                             /* 160 */
   "operation-expected",               /* 161 */
   "setting",                          /* 162 */
   "silenced",                         /* 163 */
   "tracking-value",                   /* 164 */
   "zone-members",                     /* 165 */
   "life-safety-alarm-values",         /* 166 */
   "max-segments-accepted",            /* 167 */
   "profile-name",                     /* 168 */
   "auto-slave-discovery",             /* 169 */
   "manual-slave-address-binding",     /* 170 */
   "slave-address-binding",            /* 171 */
   "slave-proxy-enable",               /* 172 */
   "last-notify-record",               /* 173 */
   "schedule-default",                 // 174
   "accepted-modes",                   // 175
   "adjust-value",                     // 176
   "count",                            // 177
   "count-before-change",              // 178
   "count-change-time",                // 179
   "cov-period",                       // 180
   "input-reference",                  // 181
   "limit-monitoring-interval",        // 182
   "logging-object",                   // 183
   "logging-record",                   // 184
   "prescale",                         // 185
   "pulse-rate",                       // 186
   "scale",                            // 187
   "scale-factor",                     // 188
   "update-time",                      // 189
   "value-before-change",              // 190
   "value-set",                        // 191
   "value-change-time",                // 192 last in 135-2004 (protocol revision 4)
   // added addendum b (135-2004)
   "align-intervals",                  // 193
   "prop-id-194",                      // 194 undefined in 135-2012
   "interval-offset",                  // 195
   "last-restart-reason",              // 196
   "logging-type",                     // 197
   "prop-id-198",                      // 198 undefined in 135-2012
   "prop-id-199",                      // 199 undefined in 135-2012
   "prop-id-200",                      // 200 undefined in 135-2012
   "prop-id-201",                      // 201 undefined in 135-2012
   "restart-notification-recipients",  // 202
   "time-of-device-restart",           // 203
   "time-synchronization-interval",    // 204
   "trigger",                          // 205
   "utc-time-synchronization-recipients",// 206
   // added by addenda d
   "node-subtype",                     // 207
   "node-type",                        // 208
   "structured-object-list",           // 209
   "subordinate-annotations",          // 210
   "subordinate-list",                 // 211
   // added by addendum e 135-2004
   "actual-shed-level",                // 212
   "duty-window",                      // 213
   "expected-shed-level",              // 214
   "full-duty-baseline",               // 215
   "prop-id-216",                      // 216 undefined in 135-2012
   "prop-id-217",                      // 217 undefined in 135-2012
   "requested-shed-level",             // 218
   "shed-duration",                    // 219
   "shed-level-descriptions",          // 220
   "shed-levels",                      // 221
   "state-description",                // 222
   "prop-id-223",                      // 223 undefined in 135-2012
   "prop-id-224",                      // 224 undefined in 135-2012
   "prop-id-225",                      // 225 undefined in 135-2012

   /* enumerations 226-235 are used in Addendum f to ANSI/ASHRAE 135-2004 */
   "door-alarm-state",                 // 226
   "door-extended-pulse-time",
   "door-members",
   "door-open-too-long-time",
   "door-pulse-time",                  // 230
   "door-status",
   "door-unlock-delay-time",
   "lock-status",
   "masked-alarm-values",
   "secured-status",                   // 235 last in 135-2008 (protocol revision 7)
   // Contributions from the bacnet-stack project http://sourceforge.net/projects/bacnet/develop

   "prop-id-236",                      // 236 undefined in 135-2012
   "prop-id-237",                      // 237 undefined in 135-2012
   "prop-id-238",                      // 238 undefined in 135-2012
   "prop-id-239",                      // 239 undefined in 135-2012
   "prop-id-240",                      // 240 undefined in 135-2012
   "prop-id-241",                      // 241 undefined in 135-2012
   "prop-id-242",                      // 242 undefined in 135-2012
   "prop-id-243",                      // 243 undefined in 135-2012
   
   /* enumerations 244-311 are defined in Addendum j to ANSI/ASHRAE 135-2004 */
   "absentee-limit",                   // 244
   "access-alarm-events",
   "access-doors",
   "access-event",
   "access-event-authentication-factor",
   "access-event-credential",
   "access-event-time",
   "access-transaction-events",
   "accompaniment",
   "accompaniment-time",
   "activation-time",
   "active-authentication-policy",
   "assigned-access-rights",
   "authentication-factors",
   "authentication-policy-list",
   "authentication-policy-names",
   "authentication-status",
   "authorization-mode",
   "belongs-to",
   "credential-disable",
   "credential-status",
   "credentials",
   "credentials-in-zone",
   "days-remaining",
   "entry-points",
   "exit-points",
   "expiry-time",
   "extended-time-enable",
   "failed-attempt-events",
   "failed-attempts",
   "failed-attempts-time",
   "last-access-event",
   "last-access-point",
   "last-credential-added",
   "last-credential-added-time",
   "last-credential-removed",
   "last-credential-removed-time",
   "last-use-time",
   "lockout",
   "lockout-relinquish-time",          // 283
   "master-exemption",                 // deleted by 135-2010ae Protocol revision 13
   "max-failed-attempts",              // 285
   "members",
   "muster-point",
   "negative-access-rules",
   "number-of-authentication-policies",
   "occupancy-count",                  // 290
   "occupancy-count-adjust",
   "occupancy-count-enable",
   "occupancy-exemption",              // 293 deleted by 135-2010ae Protocol revision 13
   "occupancy-lower-limit",
   "occupancy-lower-limit-enforced",
   "occupancy-state",
   "occupancy-upper-limit",
   "occupancy-upper-limit-enforced",
   "passback-exemption",               // 299 deleted by 135-2010ae Protocol revision 13
   "passback-mode",
   "passback-timeout",
   "positive-access-rules",
   "reason-for-disable",
   "supported-formats",
   "supported-format-classes",
   "threat-authority",
   "threat-level",
   "trace-flag",
   "transaction-notification-class",
   "user-external-identifier",
   "user-information-reference",       // 311
   /* enumerations 312-316 are unassigned in 135-2012 */
   "prop-id-312",
   "prop-id-313",
   "prop-id-314",
   "prop-id-315",
   "prop-id-316",

   /* enumerations 317-323 are used in Addendum j to ANSI/ASHRAE 135-2004 */
   "user-name",                        // 317
   "user-type",
   "uses-remaining",
   "zone-from",
   "zone-to",
   "access-event-tag",
   "global-identifier",                // 323

   /* enumerations 324-325 are unassigned in 135-2012 */
   "prop-id-324",
   "prop-id-325",
    /* enumeration 326 is used in Addendum j to ANSI/ASHRAE 135-2004 */
   "verification-time",                // 326

   "base-device-security-policy",      // 327
   "distribution-key-revision",        // 328
   "do-not-hide",                      // 329
   "key-sets",                         // 330
   "last-key-server",                  // 331
   "network-access-security-policies", // 332
   "packet-reorder-time",              // 333
   "security-pdu-timeout",             // 334
   "security-time-window",             // 335
   "supported-security-algorithms",    // 336
   "update-key-set-timeout",           // 337
   "backup-and-restore-state",         // 338
   "backup-preparation-time",          // 339
   "restore-completion-time",          // 340
   "restore-preparation-time",         // 341
   "bit-mask",                         // 342
   "bit-text",                         // 343
   "is-utc",                           // 344
   "group-members",                    // 345
   "group-member-names",               // 346
   "member-status-flags",              // 347
   "requested-update-interval",        // 348
   "covu-period",                      // 349
   "covu-recipients",                  // 350
   "event-message-texts",              // 351 last in 135-2010 (protocol revision 12)
   "event-message-texts-config",       // 352 135-2010-af.  Protocol revision 13
   "event-detection-enable",           // 353 135-2010-af.  Protocol revision 13
   "event-algorithm-inhibit",          // 354 135-2010-af.  Protocol revision 13
   "event-algorithm-inhibit-ref",      // 355 135-2010-af.  Protocol revision 13
   "time-delay-normal",                // 356 135-2010-af.  Protocol revision 13
   "reliability-evaluation-inhibit",   // 357 135-2010-af.  Protocol revision 13
   "fault-parameters",                 // 358
   "fault-type",                       // 359
   "local-forwarding-only",            // 360 135-2010-af.  Protocol revision 13
   "process-identifier-filter",        // 361 135-2010-af.  Protocol revision 13
   "subscribed-recipients",            // 362 135-2010-af.  Protocol revision 13
   "port-filter",                      // 363
   "authorization-exemptions",         // 364 135-2010-ae.  Last in Protocol revision 13
   "allow-group-delay-inhibit",        // 365 135-2010-aa.  Protocol revision 14
   "channel-number",                   // 366
   "control-groups",                   // 367
   "execution-delay",                  // 368
   "last-priority",                    // 369
   "write-status",                     // 370
   "property-list",                    // 371
   "serial-number",                    // 372
   "blink-warn-enable",                // 373
   "default-fade-time",                // 374
   "default-ramp-rate",                // 375
   "default-step-increment",           // 376
   "egress-time",                      // 377
   "in-progress",                      // 378
   "instantaneous-power",              // 379
   "lighting-command",                 // 380
   "lighting-command-default-priority",// 381
   "max-actual-value",                 // 382
   "min-actual-value",                 // 383
   "power",                            // 384
   "transition",                       // 385
   "egress-active",                    // 386 last in 135-2012 (protocol revision 14)

   // If you add a property here, you should also add it to Propid.h and
   // the other locations indicated there.
};
BAC_STRINGTABLE_EX(BACnetPropertyIdentifier, 512, 0x7FFFFFFF);

STRING_TABLE BACnetPropertyStates[] = {
   "Boolean-value",
   "Binary-value",
   "Event-type",
   "Polarity",
   "Program-change",
   "Program-state",
   "Reason-for-halt",
   "Reliability",
   "State",
   "System-status",
   "Units",
   "Unsigned-value",
   "Life-safety-mode",
   "Life-safety-state",
   "Restart-reason",
   "Door-alarm-state",
};
BAC_STRINGTABLE_EX(BACnetPropertyStates, 64, 256);

STRING_TABLE BACnetReliability[] = {
   "no-fault-detected",
   "no-sensor",
   "over-range",
   "under-range",
   "open-loop",
   "shorted-loop",
   "no-output",
   "unreliable-other",
   "process-error",
   "multi-state-fault",
   "configuration-error", // 10
   // added addendum B (135-2004)
   "member-fault",
   "communication-failure",   // 12 last in 135-2008
   "member-fault",      // 13
   "monitored-object-fault",
   "tripped",           // 15 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetReliability, 64, 65536);

STRING_TABLE BACnetRestartReason[] = {
   "unknown",
   "coldstart",
   "warmstart",
   "detected-power-lost",
   "detected-power-off",
   "hardware-watchdog",
   "software-watchdog",
   "suspended",         // 7 last in 135-2012
};
BAC_STRINGTABLE_EX(BACnetRestartReason, 64, 256);

STRING_TABLE BACnetResultFlags[] = {
   "first-item",
   "last-item",
   "more-items"
};
BAC_STRINGTABLE(BACnetResultFlags);

STRING_TABLE BACnetShedState[] = {
   "shed-inactive",
   "shed-request-pending",
   "shed-compliant",
   "shed-non-compliant",
};
BAC_STRINGTABLE(BACnetShedState);

STRING_TABLE BACnetSegmentation[] = {
   "segmented-both",
   "segmented-transmit",
   "segmented-receive",
   "no-segmentation"
};
BAC_STRINGTABLE(BACnetSegmentation);

// This list is also used to create the hash table for scripting.
STRING_TABLE BACnetServicesSupported[] = {
/* Alarm and Event Services */
   "AcknowledgeAlarm",              /* 0 */
   "ConfirmedCOVNotification",      /* 1 */
   "ConfirmedEventNotification",    /* 2 */
   "GetAlarmSummary",               /* 3 */
   "GetEnrollmentSummary",          /* 4 */
   "SubscribeCOV",                  /* 5 */

/* File Access Services */
   "AtomicReadFile",                /* 6 */
   "AtomicWriteFile",               /* 7 */

/* Object Access Services */
   "AddListElement",                /* 8 */
   "RemoveListElement",             /* 9 */
   "CreateObject",                  /* 10 */
   "DeleteObject",                  /* 11 */
   "ReadProperty",                  /* 12 */
   "ReadPropertyConditional",       /* 13 */
   "ReadPropertyMultiple",          /* 14 */
   "WriteProperty",                 /* 15 */
   "WritePropertyMultiple",         /* 16 */

/* Remote Device Management Services */
   "DeviceCommunicationControl",    /* 17 */
   "ConfirmedPrivateTransfer",      /* 18 */
   "ConfirmedTextMessage",          /* 19 */
   "ReinitializeDevice",            /* 20 */

/* Virtual Terminal Services */
   "VT-Open",                       /* 21 */
   "VT-Close",                      /* 22 */
   "VT-Data",                       /* 23 */

/*  Security Services */
   "Authenticate",                  /* 24 */
   "RequestKey",                    /* 25 */

/* Unconfirmed Services */
   "I-Am",                          /* 26 */
   "I-Have",                        /* 27 */
   "UnconfirmedCOVNotification",    /* 28 */
   "UnconfirmedEventNotification",  /* 29 */
   "UnconfirmedPrivateTransfer",    /* 30 */
   "UnconfirmedTextMessage",        /* 31 */
   "TimeSynchronization",           /* 32 */
   "Who-Has",                       /* 33 */
   "Who-Is",                        /* 34 */

/* Added after 1995 */
   "ReadRange",                     /* 35 */
   "UtcTimeSynchronization"  ,      /* 36 */
   "LifeSafetyOperation",           /* 37 */
   "SubscribeCOVProperty",          /* 38 */ 
   "GetEventInformation",           /* 39 last in 135-2008 (rev 7), 135-2010 (rev 12) */
   "WriteGroup"                     /* 40 last in 135-2012 (rev 14) */
   // Also update Vtsapi32.cpp StandardServices
};
BAC_STRINGTABLE(BACnetServicesSupported);

STRING_TABLE  BACnetSilencedState[] = {
   "unsilenced",
   "audible-silenced",
   "visible-silenced",
   "all-silenced"
};
BAC_STRINGTABLE_EX(BACnetSilencedState, 64, 65536);

STRING_TABLE BACnetStatusFlags[] = {
   "in-alarm",
   "fault",
   "overridden",
   "out-of-service"
};
BAC_STRINGTABLE(BACnetStatusFlags);

// Use the VTS utility VendorIdTable to update this table.
STRING_TABLE BACnetVendorID[] = {
   "ASHRAE",                                                 // 0
   "NIST",                                                   // 1
   "The Trane Company",                                      // 2
   "McQuay International",                                   // 3
   "PolarSoft",                                              // 4
   "Johnson Controls, Inc.",                                 // 5
   "American Auto-Matrix",                                   // 6
   "Siemens Schweiz AG (Formerly: Landis & Staefa Division Europe)", // 7
   "Delta Controls",                                         // 8
   "Siemens Schweiz AG",                                     // 9
   "Schneider Electric",                                     // 10
   "TAC",                                                    // 11
   "Orion Analysis Corporation",                             // 12
   "Teletrol Systems Inc.",                                  // 13
   "Cimetrics Technology",                                   // 14
   "Cornell University",                                     // 15
   "United Technologies Carrier",                            // 16
   "Honeywell Inc.",                                         // 17
   "Alerton / Honeywell",                                    // 18
   "TAC AB",                                                 // 19
   "Hewlett-Packard Company",                                // 20
   "Dorsette’s Inc.",                                        // 21
   "Siemens Schweiz AG (Formerly: Cerberus AG)",             // 22
   "York Controls Group",                                    // 23
   "Automated Logic Corporation",                            // 24
   "CSI Control Systems International",                      // 25
   "Phoenix Controls Corporation",                           // 26
   "Innovex Technologies, Inc.",                             // 27
   "KMC Controls, Inc.",                                     // 28
   "Xn Technologies, Inc.",                                  // 29
   "Hyundai Information Technology Co., Ltd.",               // 30
   "Tokimec Inc.",                                           // 31
   "Simplex",                                                // 32
   "North Building Technologies Limited",                    // 33
   "Notifier",                                               // 34
   "Reliable Controls Corporation",                          // 35
   "Tridium Inc.",                                           // 36
   "Sierra Monitor Corporation",                             // 37
   "Silicon Energy",                                         // 38
   "Kieback & Peter GmbH & Co KG",                           // 39
   "Anacon Systems, Inc.",                                   // 40
   "Systems Controls & Instruments, LLC",                    // 41
   "Acuity Brands Lighting, Inc.",                           // 42
   "Micropower Manufacturing",                               // 43
   "Matrix Controls",                                        // 44
   "METALAIRE",                                              // 45
   "ESS Engineering",                                        // 46
   "Sphere Systems Pty Ltd.",                                // 47
   "Walker Technologies Corporation",                        // 48
   "H I Solutions, Inc.",                                    // 49
   "MBS GmbH",                                               // 50
   "SAMSON AG",                                              // 51
   "Badger Meter Inc.",                                      // 52
   "DAIKIN Industries Ltd.",                                 // 53
   "NARA Controls Inc.",                                     // 54
   "Mammoth Inc.",                                           // 55
   "Liebert Corporation",                                    // 56
   "SEMCO Incorporated",                                     // 57
   "Air Monitor Corporation",                                // 58
   "TRIATEK, LLC",                                           // 59
   "NexLight",                                               // 60
   "Multistack",                                             // 61
   "TSI Incorporated",                                       // 62
   "Weather-Rite, Inc.",                                     // 63
   "Dunham-Bush",                                            // 64
   "Reliance Electric",                                      // 65
   "LCS Inc.",                                               // 66
   "Regulator Australia PTY Ltd.",                           // 67
   "Touch-Plate Lighting Controls",                          // 68
   "Amann GmbH",                                             // 69
   "RLE Technologies",                                       // 70
   "Cardkey Systems",                                        // 71
   "SECOM Co., Ltd.",                                        // 72
   "ABB Gebäudetechnik AG Bereich NetServ",                  // 73
   "KNX Association cvba",                                   // 74
   "Institute of Electrical Installation Engineers of Japan (IEIEJ)", // 75
   "Nohmi Bosai, Ltd.",                                      // 76
   "Carel S.p.A.",                                           // 77
   "UTC Fire & Security España, S.L.",                       // 78
   "Hochiki Corporation",                                    // 79
   "Fr. Sauter AG",                                          // 80
   "Matsushita Electric Works, Ltd.",                        // 81
   "Mitsubishi Electric Corporation, Inazawa Works",         // 82
   "Mitsubishi Heavy Industries, Ltd.",                      // 83
   "Xylem, Inc.",                                            // 84
   "Yamatake Building Systems Co., Ltd.",                    // 85
   "The Watt Stopper, Inc.",                                 // 86
   "Aichi Tokei Denki Co., Ltd.",                            // 87
   "Activation Technologies, LLC",                           // 88
   "Saia-Burgess Controls, Ltd.",                            // 89
   "Hitachi, Ltd.",                                          // 90
   "Novar Corp./Trend Control Systems Ltd.",                 // 91
   "Mitsubishi Electric Lighting Corporation",               // 92
   "Argus Control Systems, Ltd.",                            // 93
   "Kyuki Corporation",                                      // 94
   "Richards-Zeta Building Intelligence, Inc.",              // 95
   "Scientech R&D, Inc.",                                    // 96
   "VCI Controls, Inc.",                                     // 97
   "Toshiba Corporation",                                    // 98
   "Mitsubishi Electric Corporation Air Conditioning & Refrigeration Systems Works", // 99
   "Custom Mechanical Equipment, LLC",                       // 100
   "ClimateMaster",                                          // 101
   "ICP Panel-Tec, Inc.",                                    // 102
   "D-Tek Controls",                                         // 103
   "NEC Engineering, Ltd.",                                  // 104
   "PRIVA BV",                                               // 105
   "Meidensha Corporation",                                  // 106
   "JCI Systems Integration Services",                       // 107
   "Freedom Corporation",                                    // 108
   "Neuberger Gebäudeautomation GmbH",                       // 109
   "eZi Controls",                                           // 110
   "Leviton Manufacturing",                                  // 111
   "Fujitsu Limited",                                        // 112
   "Vertiv (Formerly Emerson Network Power)",                // 113
   "S. A. Armstrong, Ltd.",                                  // 114
   "Visonet AG",                                             // 115
   "M&M Systems, Inc.",                                      // 116
   "Custom Software Engineering",                            // 117
   "Nittan Company, Limited",                                // 118
   "Elutions Inc. (Wizcon Systems SAS)",                     // 119
   "Pacom Systems Pty., Ltd.",                               // 120
   "Unico, Inc.",                                            // 121
   "Ebtron, Inc.",                                           // 122
   "Scada Engine",                                           // 123
   "Lenze Americas (Formerly: AC Technology Corporation)",   // 124
   "Eagle Technology",                                       // 125
   "Data Aire, Inc.",                                        // 126
   "ABB, Inc.",                                              // 127
   "Transbit Sp. z o. o.",                                   // 128
   "Toshiba Carrier Corporation",                            // 129
   "Shenzhen Junzhi Hi-Tech Co., Ltd.",                      // 130
   "Tokai Soft",                                             // 131
   "Blue Ridge Technologies",                                // 132
   "Veris Industries",                                       // 133
   "Centaurus Prime",                                        // 134
   "Sand Network Systems",                                   // 135
   "Regulvar, Inc.",                                         // 136
   "AFDtek Division of Fastek International Inc.",           // 137
   "PowerCold Comfort Air Solutions, Inc.",                  // 138
   "I Controls",                                             // 139
   "Viconics Electronics, Inc.",                             // 140
   "Yaskawa America, Inc.",                                  // 141
   "DEOS control systems GmbH",                              // 142
   "Digitale Mess- und Steuersysteme AG",                    // 143
   "Fujitsu General Limited",                                // 144
   "Project Engineering S.r.l.",                             // 145
   "Sanyo Electric Co., Ltd.",                               // 146
   "Integrated Information Systems, Inc.",                   // 147
   "Temco Controls, Ltd.",                                   // 148
   "Airtek International Inc.",                              // 149
   "Advantech Corporation",                                  // 150
   "Titan Products, Ltd.",                                   // 151
   "Regel Partners",                                         // 152
   "National Environmental Product",                         // 153
   "Unitec Corporation",                                     // 154
   "Kanden Engineering Company",                             // 155
   "Messner Gebäudetechnik GmbH",                            // 156
   "Integrated.CH",                                          // 157
   "Price Industries",                                       // 158
   "SE-Elektronic GmbH",                                     // 159
   "Rockwell Automation",                                    // 160
   "Enflex Corp.",                                           // 161
   "ASI Controls",                                           // 162
   "SysMik GmbH Dresden",                                    // 163
   "HSC Regelungstechnik GmbH",                              // 164
   "Smart Temp Australia Pty.  Ltd.",                        // 165
   "Cooper Controls",                                        // 166
   "Duksan Mecasys Co., Ltd.",                               // 167
   "Fuji IT Co., Ltd.",                                      // 168
   "Vacon Plc",                                              // 169
   "Leader Controls",                                        // 170
   "Cylon Controls, Ltd.",                                   // 171
   "Compas",                                                 // 172
   "Mitsubishi Electric Building Techno-Service Co., Ltd.",  // 173
   "Building Control Integrators",                           // 174
   "ITG Worldwide (M) Sdn Bhd",                              // 175
   "Lutron Electronics Co., Inc.",                           // 176
   "Cooper-Atkins Corporation",                              // 177
   "LOYTEC Electronics GmbH",                                // 178
   "ProLon",                                                 // 179
   "Mega Controls Limited",                                  // 180
   "Micro Control Systems, Inc.",                            // 181
   "Kiyon, Inc.",                                            // 182
   "Dust Networks",                                          // 183
   "Advanced Building Automation Systems",                   // 184
   "Hermos AG",                                              // 185
   "CEZIM",                                                  // 186
   "Softing",                                                // 187
   "Lynxspring, Inc.",                                       // 188
   "Schneider Toshiba Inverter Europe",                      // 189
   "Danfoss Drives A/S",                                     // 190
   "Eaton Corporation",                                      // 191
   "Matyca S.A.",                                            // 192
   "Botech AB",                                              // 193
   "Noveo, Inc.",                                            // 194
   "AMEV",                                                   // 195
   "Yokogawa Electric Corporation",                          // 196
   "GFR Gesellschaft für Regelungstechnik",                  // 197
   "Exact Logic",                                            // 198
   "Mass Electronics Pty Ltd dba Innotech Control Systems Australia", // 199
   "Kandenko Co., Ltd.",                                     // 200
   "DTF, Daten-Technik Fries",                               // 201
   "Klimasoft, Ltd.",                                        // 202
   "Toshiba Schneider Inverter Corporation",                 // 203
   "Control Applications, Ltd.",                             // 204
   "KDT Systems Co., Ltd.",                                  // 205
   "Onicon Incorporated",                                    // 206
   "Automation Displays, Inc.",                              // 207
   "Control Solutions, Inc.",                                // 208
   "Remsdaq Limited",                                        // 209
   "NTT Facilities, Inc.",                                   // 210
   "VIPA GmbH",                                              // 211
   "TSC21 Association of Japan",                             // 212
   "Strato Automation",                                      // 213
   "HRW Limited",                                            // 214
   "Lighting Control & Design, Inc.",                        // 215
   "Mercy Electronic and Electrical Industries",             // 216
   "Samsung SDS Co., Ltd",                                   // 217
   "Impact Facility Solutions, Inc.",                        // 218
   "Aircuity",                                               // 219
   "Control Techniques, Ltd.",                               // 220
   "OpenGeneral Pty., Ltd.",                                 // 221
   "WAGO Kontakttechnik GmbH & Co. KG",                      // 222
   "Cerus Industrial",                                       // 223
   "Chloride Power Protection Company",                      // 224
   "Computrols, Inc.",                                       // 225
   "Phoenix Contact GmbH & Co. KG",                          // 226
   "Grundfos Management A/S",                                // 227
   "Ridder Drive Systems",                                   // 228
   "Soft Device SDN BHD",                                    // 229
   "Integrated Control Technology Limited",                  // 230
   "AIRxpert Systems, Inc.",                                 // 231
   "Microtrol Limited",                                      // 232
   "Red Lion Controls",                                      // 233
   "Digital Electronics Corporation",                        // 234
   "Ennovatis GmbH",                                         // 235
   "Serotonin Software Technologies, Inc.",                  // 236
   "LS Industrial Systems Co., Ltd.",                        // 237
   "Square D Company",                                       // 238
   "S Squared Innovations, Inc.",                            // 239
   "Aricent Ltd.",                                           // 240
   "EtherMetrics, LLC",                                      // 241
   "Industrial Control Communications, Inc.",                // 242
   "Paragon Controls, Inc.",                                 // 243
   "A. O. Smith Corporation",                                // 244
   "Contemporary Control Systems, Inc.",                     // 245
   "Intesis Software SLU",                                   // 246
   "Ingenieurgesellschaft N. Hartleb mbH",                   // 247
   "Heat-Timer Corporation",                                 // 248
   "Ingrasys Technology, Inc.",                              // 249
   "Costerm Building Automation",                            // 250
   "WILO SE",                                                // 251
   "Embedia Technologies Corp.",                             // 252
   "Technilog",                                              // 253
   "HR Controls Ltd. & Co. KG",                              // 254
   "Lennox International, Inc.",                             // 255
   "RK-Tec Rauchklappen-Steuerungssysteme GmbH & Co. KG",    // 256
   "Thermomax, Ltd.",                                        // 257
   "ELCON Electronic Control, Ltd.",                         // 258
   "Larmia Control AB",                                      // 259
   "BACnet Stack at SourceForge",                            // 260
   "G4S Security Services A/S",                              // 261
   "Exor International S.p.A.",                              // 262
   "Cristal Controles",                                      // 263
   "Regin AB",                                               // 264
   "Dimension Software, Inc. ",                              // 265
   "SynapSense Corporation",                                 // 266
   "Beijing Nantree Electronic Co., Ltd.",                   // 267
   "Camus Hydronics Ltd.",                                   // 268
   "Kawasaki Heavy Industries, Ltd. ",                       // 269
   "Critical Environment Technologies",                      // 270
   "ILSHIN IBS Co., Ltd.",                                   // 271
   "ELESTA Energy Control AG",                               // 272
   "KROPMAN Installatietechniek",                            // 273
   "Baldor Electric Company",                                // 274
   "INGA mbH",                                               // 275
   "GE Consumer & Industrial",                               // 276
   "Functional Devices, Inc.",                               // 277
   "ESAC",                                                   // 278
   "M-System Co., Ltd.",                                     // 279
   "Yokota Co., Ltd.",                                       // 280
   "Hitranse Technology Co., LTD",                           // 281
   "Vigilent Corporation",                                   // 282
   "Kele, Inc.",                                             // 283
   "Opera Electronics, Inc.",                                // 284
   "Gentec",                                                 // 285
   "Embedded Science Labs, LLC",                             // 286
   "Parker Hannifin Corporation",                            // 287
   "MaCaPS International Limited",                           // 288
   "Link4 Corporation",                                      // 289
   "Romutec Steuer-u. Regelsysteme GmbH ",                   // 290
   "Pribusin, Inc.",                                         // 291
   "Advantage Controls",                                     // 292
   "Critical Room Control",                                  // 293
   "LEGRAND",                                                // 294
   "Tongdy Control Technology Co., Ltd. ",                   // 295
   "ISSARO Integrierte Systemtechnik",                       // 296
   "Pro-Dev Industries",                                     // 297
   "DRI-STEEM",                                              // 298
   "Creative Electronic GmbH",                               // 299
   "Swegon AB",                                              // 300
   "Jan Brachacek",                                          // 301
   "Hitachi Appliances, Inc.",                               // 302
   "Real Time Automation, Inc.",                             // 303
   "ITEC Hankyu-Hanshin Co.",                                // 304
   "Cyrus E&M Engineering Co., Ltd. ",                       // 305
   "Badger Meter",                                           // 306
   "Cirrascale Corporation",                                 // 307
   "Elesta GmbH Building Automation ",                       // 308
   "Securiton",                                              // 309
   "OSlsoft, Inc.",                                          // 310
   "Hanazeder Electronic GmbH ",                             // 311
   "Honeywell Security Deutschland, Novar GmbH",             // 312
   "Siemens Industry, Inc.",                                 // 313
   "ETM Professional Control GmbH",                          // 314
   "Meitav-tec, Ltd.",                                       // 315
   "Janitza Electronics GmbH ",                              // 316
   "MKS Nordhausen",                                         // 317
   "De Gier Drive Systems B.V. ",                            // 318
   "Cypress Envirosystems",                                  // 319
   "SMARTron s.r.o.",                                        // 320
   "Verari Systems, Inc.",                                   // 321
   "K-W Electronic Service, Inc.",                           // 322
   "ALFA-SMART Energy Management",                           // 323
   "Telkonet, Inc.",                                         // 324
   "Securiton GmbH",                                         // 325
   "Cemtrex, Inc.",                                          // 326
   "Performance Technologies, Inc.",                         // 327
   "Xtralis (Aust) Pty Ltd",                                 // 328
   "TROX GmbH",                                              // 329
   "Beijing Hysine Technology Co., Ltd",                     // 330
   "RCK Controls, Inc.",                                     // 331
   "Distech Controls SAS",                                   // 332
   "Novar/Honeywell",                                        // 333
   "The S4 Group, Inc.",                                     // 334
   "Schneider Electric",                                     // 335
   "LHA Systems",                                            // 336
   "GHM engineering Group, Inc.",                            // 337
   "Cllimalux S.A.",                                         // 338
   "VAISALA Oyj",                                            // 339
   "COMPLEX (Beijing) Technology, Co., LTD.",                // 340
   "SCADAmetrics",                                           // 341
   "POWERPEG NSI Limited",                                   // 342
   "BACnet Interoperability Testing Services, Inc.",         // 343
   "Teco a.s.",                                              // 344
   "Plexus Technology, Inc.",                                // 345
   "Energy Focus, Inc.",                                     // 346
   "Powersmiths International Corp.",                        // 347
   "Nichibei Co., Ltd.",                                     // 348
   "HKC Technology Ltd.",                                    // 349
   "Ovation Networks, Inc.",                                 // 350
   "Setra Systems",                                          // 351
   "AVG Automation",                                         // 352
   "ZXC Ltd.",                                               // 353
   "Byte Sphere",                                            // 354
   "Generiton Co., Ltd.",                                    // 355
   "Holter Regelarmaturen GmbH & Co. KG",                    // 356
   "Bedford Instruments, LLC",                               // 357
   "Standair Inc.",                                          // 358
   "WEG Automation - R&D",                                   // 359
   "Prolon Control Systems ApS",                             // 360
   "Inneasoft",                                              // 361
   "ConneXSoft GmbH",                                        // 362
   "CEAG Notlichtsysteme GmbH",                              // 363
   "Distech Controls Inc.",                                  // 364
   "Industrial Technology Research Institute",               // 365
   "ICONICS, Inc.",                                          // 366
   "IQ Controls s.c.",                                       // 367
   "OJ Electronics A/S",                                     // 368
   "Rolbit Ltd.",                                            // 369
   "Synapsys Solutions Ltd.",                                // 370
   "ACME Engineering Prod. Ltd.",                            // 371
   "Zener Electric Pty, Ltd.",                               // 372
   "Selectronix, Inc.",                                      // 373
   "Gorbet & Banerjee, LLC.",                                // 374
   "IME",                                                    // 375
   "Stephen H. Dawson Computer Service",                     // 376
   "Accutrol, LLC",                                          // 377
   "Schneider Elektronik GmbH",                              // 378
   "Alpha-Inno Tec GmbH",                                    // 379
   "ADMMicro, Inc.",                                         // 380
   "Greystone Energy Systems, Inc.",                         // 381
   "CAP Technologie",                                        // 382
   "KeRo Systems",                                           // 383
   "Domat Control System s.r.o.",                            // 384
   "Efektronics Pty. Ltd.",                                  // 385
   "Hekatron Vertriebs GmbH",                                // 386
   "Securiton AG",                                           // 387
   "Carlo Gavazzi Controls SpA",                             // 388
   "Chipkin Automation Systems",                             // 389
   "Savant Systems, LLC",                                    // 390
   "Simmtronic Lighting Controls",                           // 391
   "Abelko Innovation AB",                                   // 392
   "Seresco Technologies Inc.",                              // 393
   "IT Watchdogs",                                           // 394
   "Automation Assist Japan Corp.",                          // 395
   "Thermokon Sensortechnik GmbH",                           // 396
   "EGauge Systems, LLC",                                    // 397
   "Quantum Automation (ASIA) PTE, Ltd.",                    // 398
   "Toshiba Lighting & Technology Corp.",                    // 399
   "SPIN Engenharia de Automação Ltda.",                     // 400
   "Logistics Systems & Software Services India PVT. Ltd.",  // 401
   "Delta Controls Integration Products",                    // 402
   "Focus Media",                                            // 403
   "LUMEnergi Inc.",                                         // 404
   "Kara Systems",                                           // 405
   "RF Code, Inc.",                                          // 406
   "Fatek Automation Corp.",                                 // 407
   "JANDA Software Company, LLC",                            // 408
   "Open System Solutions Limited",                          // 409
   "Intelec Systems PTY Ltd.",                               // 410
   "Ecolodgix, LLC",                                         // 411
   "Douglas Lighting Controls",                              // 412
   "iSAtech GmbH",                                           // 413
   "AREAL",                                                  // 414
   "Beckhoff Automation GmbH",                               // 415
   "IPAS GmbH",                                              // 416
   "KE2 Therm Solutions",                                    // 417
   "Base2Products",                                          // 418
   "DTL Controls, LLC",                                      // 419
   "INNCOM International, Inc.",                             // 420
   "BTR Netcom GmbH",                                        // 421
   "Greentrol Automation, Inc",                              // 422
   "BELIMO Automation AG",                                   // 423
   "Samsung Heavy Industries Co, Ltd",                       // 424
   "Triacta Power Technologies, Inc.",                       // 425
   "Globestar Systems",                                      // 426
   "MLB Advanced Media, LP",                                 // 427
   "SWG Stuckmann Wirtschaftliche Gebäudesysteme GmbH",      // 428
   "SensorSwitch",                                           // 429
   "Multitek Power Limited",                                 // 430
   "Aquametro AG",                                           // 431
   "LG Electronics Inc.",                                    // 432
   "Electronic Theatre Controls, Inc.",                      // 433
   "Mitsubishi Electric Corporation Nagoya Works",           // 434
   "Delta Electronics, Inc.",                                // 435
   "Elma Kurtalj, Ltd.",                                     // 436
   "ADT Fire and Security Sp. A.o.o.",                       // 437
   "Nedap Security Management",                              // 438
   "ESC Automation Inc.",                                    // 439
   "DSP4YOU Ltd.",                                           // 440
   "GE Sensing and Inspection Technologies",                 // 441
   "Embedded Systems SIA",                                   // 442
   "BEFEGA GmbH",                                            // 443
   "Baseline Inc.",                                          // 444
   "M2M Systems Integrators",                                // 445
   "OEMCtrl",                                                // 446
   "Clarkson Controls Limited",                              // 447
   "Rogerwell Control System Limited",                       // 448
   "SCL Elements",                                           // 449
   "Hitachi Ltd.",                                           // 450
   "Newron System SA",                                       // 451
   "BEVECO Gebouwautomatisering BV",                         // 452
   "Streamside Solutions",                                   // 453
   "Yellowstone Soft",                                       // 454
   "Oztech Intelligent Systems Pty Ltd.",                    // 455
   "Novelan GmbH",                                           // 456
   "Flexim Americas Corporation",                            // 457
   "ICP DAS Co., Ltd.",                                      // 458
   "CARMA Industries Inc.",                                  // 459
   "Log-One Ltd.",                                           // 460
   "TECO Electric & Machinery Co., Ltd.",                    // 461
   "ConnectEx, Inc.",                                        // 462
   "Turbo DDC Südwest",                                      // 463
   "Quatrosense Environmental Ltd.",                         // 464
   "Fifth Light Technology Ltd.",                            // 465
   "Scientific Solutions, Ltd.",                             // 466
   "Controller Area Network Solutions (M) Sdn Bhd",          // 467
   "RESOL - Elektronische Regelungen GmbH",                  // 468
   "RPBUS LLC",                                              // 469
   "BRS Sistemas Eletronicos",                               // 470
   "WindowMaster A/S",                                       // 471
   "Sunlux Technologies Ltd.",                               // 472
   "Measurlogic",                                            // 473
   "Frimat GmbH",                                            // 474
   "Spirax Sarco",                                           // 475
   "Luxtron",                                                // 476
   "Raypak Inc",                                             // 477
   "Air Monitor Corporation",                                // 478
   "Regler Och Webbteknik Sverige (ROWS)",                   // 479
   "Intelligent Lighting Controls Inc.",                     // 480
   "Sanyo Electric Industry Co., Ltd",                       // 481
   "E-Mon Energy Monitoring Products",                       // 482
   "Digital Control Systems",                                // 483
   "ATI Airtest Technologies, Inc.",                         // 484
   "SCS SA",                                                 // 485
   "HMS Industrial Networks AB",                             // 486
   "Shenzhen Universal Intellisys Co Ltd",                   // 487
   "EK Intellisys Sdn Bhd",                                  // 488
   "SysCom",                                                 // 489
   "Firecom, Inc.",                                          // 490
   "ESA Elektroschaltanlagen Grimma GmbH",                   // 491
   "Kumahira Co Ltd",                                        // 492
   "Hotraco",                                                // 493
   "SABO Elektronik GmbH",                                   // 494
   "Equip'Trans",                                            // 495
   "Temperature Control Specialities Co., Inc (TCS)",        // 496
   "FlowCon International A/S",                              // 497
   "ThyssenKrupp Elevator Americas",                         // 498
   "Abatement Technologies",                                 // 499
   "Continental Control Systems, LLC",                       // 500
   "WISAG Automatisierungstechnik GmbH & Co KG",             // 501
   "EasyIO",                                                 // 502
   "EAP-Electric GmbH",                                      // 503
   "Hardmeier",                                              // 504
   "Mircom Group of Companies",                              // 505
   "Quest Controls",                                         // 506
   "Mestek, Inc",                                            // 507
   "Pulse Energy",                                           // 508
   "Tachikawa Corporation",                                  // 509
   "University of Nebraska-Lincoln",                         // 510
   "Redwood Systems",                                        // 511
   "PASStec Industrie-Elektronik GmbH",                      // 512
   "NgEK, Inc.",                                             // 513
   "t-mac Technologies",                                     // 514
   "Jireh Energy Tech Co., Ltd.",                            // 515
   "Enlighted Inc.",                                         // 516
   "El-Piast Sp. Z o.o",                                     // 517
   "NetxAutomation Software GmbH",                           // 518
   "Invertek Drives",                                        // 519
   "Deutschmann Automation GmbH & Co. KG",                   // 520
   "EMU Electronic AG",                                      // 521
   "Phaedrus Limited",                                       // 522
   "Sigmatek GmbH & Co KG",                                  // 523
   "Marlin Controls",                                        // 524
   "Circutor, SA",                                           // 525
   "UTC Fire & Security",                                    // 526
   "DENT Instruments, Inc.",                                 // 527
   "FHP Manufacturing Company - Bosch Group",                // 528
   "GE Intelligent Platforms",                               // 529
   "Inner Range Pty Ltd",                                    // 530
   "GLAS Energy Technology",                                 // 531
   "MSR-Electronic-GmbH",                                    // 532
   "Energy Control Systems, Inc.",                           // 533
   "EMT Controls",                                           // 534
   "Daintree Networks Inc.",                                 // 535
   "EURO ICC d.o.o",                                         // 536
   "TE Connectivity Energy",                                 // 537
   "GEZE GmbH",                                              // 538
   "NEC Corporation",                                        // 539
   "Ho Cheung International Company Limited",                // 540
   "Sharp Manufacturing Systems Corporation",                // 541
   "DOT CONTROLS a.s.",                                      // 542
   "BeaconMedæs",                                            // 543
   "Midea Commercial Aircon",                                // 544
   "WattMaster Controls",                                    // 545
   "Kamstrup A/S",                                           // 546
   "CA Computer Automation GmbH",                            // 547
   "Laars Heating Systems Company",                          // 548
   "Hitachi Systems, Ltd.",                                  // 549
   "Fushan AKE Electronic Engineering Co., Ltd.",            // 550
   "Toshiba International Corporation",                      // 551
   "Starman Systems, LLC",                                   // 552
   "Samsung Techwin Co., Ltd.",                              // 553
   "ISAS-Integrated Switchgear and Systems P/L",             // 554
   "BACnet Examples",                                        // 555
   "Obvius",                                                 // 556
   "Marek Guzik",                                            // 557
   "Vortek Instruments, LLC",                                // 558
   "Universal Lighting Technologies",                        // 559
   "Myers Power Products, Inc.",                             // 560
   "Vector Controls GmbH",                                   // 561
   "Crestron Electronics, Inc.",                             // 562
   "A&E Controls Limited",                                   // 563
   "Projektomontaza A.D.",                                   // 564
   "Freeaire Refrigeration",                                 // 565
   "Aqua Cooler Pty Limited",                                // 566
   "Basic Controls",                                         // 567
   "GE Measurement and Control Solutions Advanced Sensors",  // 568
   "EQUAL Networks",                                         // 569
   "Millennial Net",                                         // 570
   "APLI Ltd",                                               // 571
   "Electro Industries/GaugeTech",                           // 572
   "SangMyung University",                                   // 573
   "Coppertree Analytics, Inc.",                             // 574
   "CoreNetiX GmbH",                                         // 575
   "Acutherm",                                               // 576
   "Dr. Riedel Automatisierungstechnik GmbH",                // 577
   "Shina System Co., Ltd",                                  // 578
   "Iqapertus",                                              // 579
   "PSE Technology",                                         // 580
   "BA Systems",                                             // 581
   "BTICINO",                                                // 582
   "Monico, Inc.",                                           // 583
   "iCue",                                                   // 584
   "tekmar Control Systems Ltd.",                            // 585
   "Control Technology Corporation",                         // 586
   "GFAE GmbH",                                              // 587
   "BeKa Software GmbH",                                     // 588
   "Isoil Industria SpA",                                    // 589
   "Home Systems Consulting SpA",                            // 590
   "Socomec",                                                // 591
   "Everex Communications, Inc.",                            // 592
   "Ceiec Electric Technology",                              // 593
   "Atrila GmbH",                                            // 594
   "WingTechs",                                              // 595
   "Shenzhen Mek Intellisys Pte Ltd.",                       // 596
   "Nestfield Co., Ltd.",                                    // 597
   "Swissphone Telecom AG",                                  // 598
   "PNTECH JSC",                                             // 599
   "Horner APG, LLC",                                        // 600
   "PVI Industries, LLC",                                    // 601
   "Ela-compil",                                             // 602
   "Pegasus Automation International LLC",                   // 603
   "Wight Electronic Services Ltd.",                         // 604
   "Marcom",                                                 // 605
   "Exhausto A/S",                                           // 606
   "Dwyer Instruments, Inc.",                                // 607
   "Link GmbH",                                              // 608
   "Oppermann Regelgerate GmbH",                             // 609
   "NuAire, Inc.",                                           // 610
   "Nortec Humidity, Inc.",                                  // 611
   "Bigwood Systems, Inc.",                                  // 612
   "Enbala Power Networks",                                  // 613
   "Inter Energy Co., Ltd.",                                 // 614
   "ETC",                                                    // 615
   "COMELEC S.A.R.L",                                        // 616
   "Pythia Technologies",                                    // 617
   "TrendPoint Systems, Inc.",                               // 618
   "AWEX",                                                   // 619
   "Eurevia",                                                // 620
   "Kongsberg E-lon AS",                                     // 621
   "FlaktWoods",                                             // 622
   "E + E Elektronik GES M.B.H.",                            // 623
   "ARC Informatique",                                       // 624
   "SKIDATA AG",                                             // 625
   "WSW Solutions",                                          // 626
   "Trefon Electronic GmbH",                                 // 627
   "Dongseo System",                                         // 628
   "Kanontec Intelligence Technology Co., Ltd.",             // 629
   "EVCO S.p.A.",                                            // 630
   "Accuenergy (Canada) Inc.",                               // 631
   "SoftDEL",                                                // 632
   "Orion Energy Systems, Inc.",                             // 633
   "Roboticsware",                                           // 634
   "DOMIQ Sp. z o.o.",                                       // 635
   "Solidyne",                                               // 636
   "Elecsys Corporation",                                    // 637
   "Conditionaire International Pty. Limited",               // 638
   "Quebec, Inc.",                                           // 639
   "Homerun Holdings",                                       // 640
   "Murata Americas",                                        // 641
   "Comptek",                                                // 642
   "Westco Systems, Inc.",                                   // 643
   "Advancis Software & Services GmbH",                      // 644
   "Intergrid, LLC",                                         // 645
   "Markerr Controls, Inc.",                                 // 646
   "Toshiba Elevator and Building Systems Corporation",      // 647
   "Spectrum Controls, Inc.",                                // 648
   "Mkservice",                                              // 649
   "Fox Thermal Instruments",                                // 650
   "SyxthSense Ltd",                                         // 651
   "DUHA System S R.O.",                                     // 652
   "NIBE",                                                   // 653
   "Melink Corporation",                                     // 654
   "Fritz-Haber-Institut",                                   // 655
   "MTU Onsite Energy GmbH, Gas Power Systems",              // 656
   "Omega Engineering, Inc.",                                // 657
   "Avelon",                                                 // 658
   "Ywire Technologies, Inc.",                               // 659
   "M.R. Engineering Co., Ltd.",                             // 660
   "Lochinvar, LLC",                                         // 661
   "Sontay Limited",                                         // 662
   "GRUPA Slawomir Chelminski",                              // 663
   "Arch Meter Corporation",                                 // 664
   "Senva, Inc.",                                            // 665
   "Beelzebub Controls",                                     // 666
   "FM-Tec",                                                 // 667
   "Systems Specialists, Inc.",                              // 668
   "SenseAir",                                               // 669
   "AB IndustrieTechnik Srl",                                // 670
   "Cortland Research, LLC",                                 // 671
   "MediaView",                                              // 672
   "VDA Elettronica",                                        // 673
   "CSS, Inc.",                                              // 674
   "Tek-Air Systems, Inc.",                                  // 675
   "ICDT",                                                   // 676
   "The Armstrong Monitoring Corporation",                   // 677
   "DIXELL S.r.l",                                           // 678
   "Lead System, Inc.",                                      // 679
   "ISM EuroCenter S.A.",                                    // 680
   "TDIS",                                                   // 681
   "Trade FIDES",                                            // 682
   "Knürr GmbH (Emerson Network Power)",                     // 683
   "Resource Data Management",                               // 684
   "Abies Technology, Inc.",                                 // 685
   "Amalva",                                                 // 686
   "MIRAE Electrical Mfg. Co., Ltd.",                        // 687
   "HunterDouglas Architectural Projects Scandinavia ApS",   // 688
   "RUNPAQ Group Co., Ltd",                                  // 689
   "Unicard SA",                                             // 690
   "IE Technologies",                                        // 691
   "Ruskin Manufacturing",                                   // 692
   "Calon Associates Limited",                               // 693
   "Contec Co., Ltd.",                                       // 694
   "iT GmbH",                                                // 695
   "Autani Corporation",                                     // 696
   "Christian Fortin",                                       // 697
   "HDL",                                                    // 698
   "IPID Sp. Z.O.O Limited",                                 // 699
   "Fuji Electric Co., Ltd",                                 // 700
   "View, Inc.",                                             // 701
   "Samsung S1 Corporation",                                 // 702
   "New Lift",                                               // 703
   "VRT Systems",                                            // 704
   "Motion Control Engineering, Inc.",                       // 705
   "Weiss Klimatechnik GmbH",                                // 706
   "Elkon",                                                  // 707
   "Eliwell Controls S.r.l.",                                // 708
   "Japan Computer Technos Corp",                            // 709
   "Rational Network ehf",                                   // 710
   "Magnum Energy Solutions, LLC",                           // 711
   "MelRok",                                                 // 712
   "VAE Group",                                              // 713
   "LGCNS",                                                  // 714
   "Berghof Automationstechnik GmbH",                        // 715
   "Quark Communications, Inc.",                             // 716
   "Sontex",                                                 // 717
   "mivune AG",                                              // 718
   "Panduit",                                                // 719
   "Smart Controls, LLC",                                    // 720
   "Compu-Aire, Inc.",                                       // 721
   "Sierra",                                                 // 722
   "ProtoSense Technologies",                                // 723
   "Eltrac Technologies Pvt Ltd",                            // 724
   "Bektas Invisible Controls GmbH",                         // 725
   "Entelec",                                                // 726
   "INNEXIV",                                                // 727
   "Covenant",                                               // 728
   "Davitor AB",                                             // 729
   "TongFang Technovator",                                   // 730
   "Building Robotics, Inc.",                                // 731
   "HSS-MSR UG",                                             // 732
   "FramTack LLC",                                           // 733
   "B. L. Acoustics, Ltd.",                                  // 734
   "Traxxon Rock Drills, Ltd",                               // 735
   "Franke",                                                 // 736
   "Wurm GmbH & Co",                                         // 737
   "AddENERGIE",                                             // 738
   "Mirle Automation Corporation",                           // 739
   "Ibis Networks",                                          // 740
   "ID-KARTA s.r.o.",                                        // 741
   "Anaren, Inc.",                                           // 742
   "Span, Incorporated",                                     // 743
   "Bosch Thermotechnology Corp",                            // 744
   "DRC Technology S.A.",                                    // 745
   "Shanghai Energy Building Technology Co, Ltd",            // 746
   "Fraport AG",                                             // 747
   "Flowgroup",                                              // 748
   "Skytron Energy, GmbH",                                   // 749
   "ALTEL Wicha, Golda Sp. J.",                              // 750
   "Drupal",                                                 // 751
   "Axiomatic Technology, Ltd",                              // 752
   "Bohnke + Partner",                                       // 753
   "Function1",                                              // 754
   "Optergy Pty, Ltd",                                       // 755
   "LSI Virticus",                                           // 756
   "Konzeptpark GmbH",                                       // 757
   "Hubbell Building Automation, Inc.",                      // 758
   "eCurv, Inc.",                                            // 759
   "Agnosys GmbH",                                           // 760
   "Shanghai Sunfull Automation Co., LTD",                   // 761
   "Kurz Instruments, Inc.",                                 // 762
   "Cias Elettronica S.r.l.",                                // 763
   "Multiaqua, Inc.",                                        // 764
   "BlueBox",                                                // 765
   "Sensidyne",                                              // 766
   "Viessmann Elektronik GmbH",                              // 767
   "ADFweb.com srl",                                         // 768
   "Gaylord Industries",                                     // 769
   "Majur Ltd.",                                             // 770
   "Shanghai Huilin Technology Co., Ltd.",                   // 771
   "Exotronic",                                              // 772
   "Safecontrol spol s.r.o.",                                // 773
   "Amatis",                                                 // 774
   "Universal Electric Corporation",                         // 775
   "iBACnet",                                                // 776
   "Reserved-by-ASHRAE-777",                                 // 777
   "Smartrise Engineering, Inc.",                            // 778
   "Miratron, Inc.",                                         // 779
   "SmartEdge",                                              // 780
   "Mitsubishi Electric Australia Pty Ltd",                  // 781
   "Triangle Research International Ptd Ltd",                // 782
   "Produal Oy",                                             // 783
   "Milestone Systems A/S",                                  // 784
   "Trustbridge",                                            // 785
   "Feedback Solutions",                                     // 786
   "IES",                                                    // 787
   "GE Critical Power",                                      // 788
   "Riptide IO",                                             // 789
   "Messerschmitt Systems AG",                               // 790
   "Dezem Energy Controlling",                               // 791
   "MechoSystems",                                           // 792
   "evon GmbH",                                              // 793
   "CS Lab GmbH",                                            // 794
   "8760 Enterprises, Inc.",                                 // 795
   "Touche Controls",                                        // 796
   "Ontrol Teknik Malzeme San. ve Tic. A.S.",                // 797
   "Uni Control System Sp. Z o.o.",                          // 798
   "Weihai Ploumeter Co., Ltd",                              // 799
   "Elcom International Pvt. Ltd",                           // 800
   "Philips Lighting",                                       // 801
   "AutomationDirect",                                       // 802
   "Paragon Robotics",                                       // 803
   "SMT System & Modules Technology AG",                     // 804
   "OS Technology Service and Trading Co., LTD",             // 805
   "CMR Controls Ltd",                                       // 806
   "Innovari, Inc.",                                         // 807
   "ABB Control Products",                                   // 808
   "Gesellschaft fur Gebäudeautomation mbH",                 // 809
   "RODI Systems Corp.",                                     // 810
   "Nextek Power Systems",                                   // 811
   "Creative Lighting",                                      // 812
   "WaterFurnace International",                             // 813
   "Mercury Security",                                       // 814
   "Hisense (Shandong) Air-Conditioning Co., Ltd.",          // 815
   "Layered Solutions, Inc.",                                // 816
   "Leegood Automatic System, Inc.",                         // 817
   "Shanghai Restar Technology Co., Ltd.",                   // 818
   "Reimann Ingenieurbüro",                                  // 819
   "LynTec",                                                 // 820
   "HTP",                                                    // 821
   "Elkor Technologies, Inc.",                               // 822
   "Bentrol Pty Ltd",                                        // 823
   "Team-Control Oy",                                        // 824
   "NextDevice, LLC",                                        // 825
   "GLOBAL CONTROL 5 Sp. z o.o.",                            // 826
   "King I Electronics Co., Ltd",                            // 827
   "SAMDAV",                                                 // 828
   "Next Gen Industries Pvt. Ltd.",                          // 829
   "Entic LLC",                                              // 830
   "ETAP",                                                   // 831
   "Moralle Electronics Limited",                            // 832
   "Leicom AG",                                              // 833
   "Watts Regulator Company",                                // 834
   "S.C. Orbtronics S.R.L.",                                 // 835
   "Gaussan Technologies",                                   // 836
   "WEBfactory GmbH",                                        // 837
   "Ocean Controls",                                         // 838
   "Messana  Air-Ray Conditioning s.r.l.",                   // 839
   "Hangzhou BATOWN Technology Co. Ltd.",                    // 840
   "Reasonable Controls",                                    // 841
   "Servisys, Inc.",                                         // 842
   "halstrup-walcher GmbH",                                  // 843
   "SWG Automation Fuzhou Limited",                          // 844
   "KSB Aktiengesellschaft",                                 // 845
   "Hybryd Sp. z o.o.",                                      // 846
   "Helvatron AG",                                           // 847
   "Oderon Sp. Z.O.O.",                                      // 848
   "miko",                                                   // 849
   "Exodraft",                                               // 850
   "Hochhuth GmbH",                                          // 851
   "Integrated System Technologies Ltd.",                    // 852
   "Shanghai Cellcons Controls Co., Ltd",                    // 853
   "Emme Controls, LLC",                                     // 854
   "Field Diagnostic Services, Inc.",                        // 855
   "Ges Teknik A.S.",                                        // 856
   "Global Power Products, Inc.",                            // 857
   "Option NV",                                              // 858
   "BV-Control AG",                                          // 859
   "Sigren Engineering AG",                                  // 860
   "Shanghai Jaltone Technology Co., Ltd.",                  // 861
   "MaxLine Solutions Ltd",                                  // 862
   "Kron Instrumentos Elétricos Ltda",                       // 863
   "Thermo Matrix",                                          // 864
   "Infinite Automation Systems, Inc.",                      // 865
   "Vantage",                                                // 866
   "Elecon Measurements Pvt Ltd",                            // 867
   "TBA",                                                    // 868
   "Carnes Company",                                         // 869
   "Harman Professional",                                    // 870
   "Nenutec Asia Pacific Pte Ltd",                           // 871
   "Gia NV",                                                 // 872
   "Kepware Tehnologies",                                    // 873
   "Temperature Electronics Ltd",                            // 874
   "Packet Power",                                           // 875
   "Project Haystack Corporation",                           // 876
   "DEOS Controls Americas Inc.",                            // 877
   "Senseware Inc",                                          // 878
   "MST Systemtechnik AG",                                   // 879
   "Lonix Ltd",                                              // 880
   "GMC-I Messtechnik GmbH",                                 // 881
   "Aviosys International Inc.",                             // 882
   "Efficient Building Automation Corp.",                    // 883
   "Accutron Instruments Inc.",                              // 884
   "Vermont Energy Control Systems LLC",                     // 885
   "DCC Dynamics",                                           // 886
   "Brück Electronic GmbH",                                  // 887
   "Reserved-by-ASHRAE-888",                                 // 888
   "NGBS Hungary Ltd.",                                      // 889
   "ILLUM Technology, LLC",                                  // 890
   "Delta Controls Germany Limited",                         // 891
   "S+T Service & Technique S.A.",                           // 892
   "SimpleSoft",                                             // 893
   "Candi Controls, Inc.",                                   // 894
   "EZEN Solution Inc.",                                     // 895
   "Fujitec Co. Ltd.",                                       // 896
   "Terralux",                                               // 897
   "Annicom",                                                // 898
   "Bihl+Wiedemann GmbH",                                    // 899
   "Draper, Inc.",                                           // 900
   "Schüco International KG",                                // 901
   "Otis Elevator Company",                                  // 902
   "Fidelix Oy",                                             // 903
   "RAM GmbH Mess- und Regeltechnik",                        // 904
   "WEMS",                                                   // 905
   "Ravel Electronics Pvt Ltd",                              // 906
   "OmniMagni",                                              // 907
   "Echelon",                                                // 908
   "Intellimeter Canada, Inc.",                              // 909
   "Bithouse Oy",                                            // 910
   "Reserved-by-ASHRAE-911",                                 // 911
   "BuildPulse",                                             // 912
   "Shenzhen 1000 Building Automation Co. Ltd",              // 913
   "AED Engineering GmbH",                                   // 914
   "Güntner GmbH & Co. KG",                                  // 915
   "KNXlogic",                                               // 916
   "CIM Environmental Group",                                // 917
   "Flow Control",                                           // 918
   "Lumen Cache, Inc.",                                      // 919
   "Ecosystem",                                              // 920
   "Potter Electric Signal Company, LLC",                    // 921
   "Tyco Fire & Security S.p.A.",                            // 922
   "Watanabe Electric Industry Co., Ltd.",                   // 923
   "Causam Energy",                                          // 924
   "W-tec AG",                                               // 925
   "IMI Hydronic Engineering International SA",              // 926
   "ARIGO Software",                                         // 927
   "MSA Safety",                                             // 928
   "Smart Solucoes Ltda - MERCATO",                          // 929
   "PIATRA Engineering",                                     // 930
   "ODIN Automation Systems, LLC",                           // 931
   "Belparts NV",                                            // 932
   "UAB, SALDA",                                             // 933
   "Alre-IT Regeltechnik GmbH",                              // 934
   "Ingenieurbüro H. Lertes GmbH & Co. KG",                  // 935
   "Breathing Buildings",                                    // 936
   "eWON SA",                                                // 937
   "Cav. Uff. Giacomo Cimberio S.p.A",                       // 938
   "PKE Electronics AG",                                     // 939
   "Allen",                                                  // 940
   "Kastle Systems",                                         // 941
   "Logical Electro-Mechanical (EM) Systems, Inc.",          // 942
   "ppKinetics Instruments, LLC",                            // 943
   "Cathexis Technologies",                                  // 944
   "Sylop sp. Z o.o. sp.k",                                  // 945
   "Brauns  Control GmbH",                                   // 946
   "Omron Corporation",                                      // 947
   "Wildeboer Bauteile Gmbh",                                // 948
   "Shanghai Biens Technologies Ltd",                        // 949
   "Beijing HZHY Technology Co., Ltd",                       // 950
   "Building Clouds",                                        // 951
   "The University of Sheffield-Department of Electronic and Electrical Engineering", // 952
   "Fabtronics Australia Pty Ltd",                           // 953
   "SLAT",                                                   // 954
   "Software Motor Corporation",                             // 955
   "Armstrong International Inc.",                           // 956
   "Steril-Aire, Inc.",                                      // 957
   "Infinique",                                              // 958
   "Arcom",                                                  // 959
   "Argo Performance, Ltd",                                  // 960
   "Dialight",                                               // 961
   "Ideal Technical Solutions",                              // 962
   "Neurobat AG",                                            // 963
   "Neyer Software Consulting LLC",                          // 964
   "SCADA Technology Development Co., Ltd.",                 // 965
   "Demand Logic Limited",                                   // 966
   "GWA Group Limited",                                      // 967
   "Occitaline",                                             // 968
   "NAO Digital Co., Ltd.",                                  // 969
   "Shenzhen Chanslink Network Technology Co., Ltd.",        // 970
   "Samsung Electronics Co., Ltd.",                          // 971
   "Mesa Laboratories, Inc.",                                // 972
   "Fischer",                                                // 973
   "OpSys Solutions Ltd.",                                   // 974
   "Advanced Devices Limited",                               // 975
   "Condair",                                                // 976
   "INELCOM Ingenieria Electronica Comercial S.A.",          // 977
   "GridPoint, Inc.",                                        // 978
   "ADF Technologies Sdn Bhd",                               // 979
   "EPM, Inc.",                                              // 980
   "Lighting Controls Ltd",                                  // 981
   "Perix Controls Ltd.",                                    // 982
   "AERCO International, Inc.",                              // 983
   "KONE Inc.",                                              // 984
   "Ziehl-Abegg SE",                                         // 985
   "Robot, S.A.",                                            // 986
   "Optigo Networks, Inc.",                                  // 987
   "Openmotics BVBA",                                        // 988
   "Metropolitan Industries, Inc.",                          // 989
   "Huawei Technologies Co., Ltd.",                          // 990
   "OSRAM Sylvania, Inc.",                                   // 991
   "Vanti",                                                  // 992
   "Cree, Inc.",                                             // 993
   "Richmond Heights SDN BHD",                               // 994
   "Payne-Sparkman Lighting Mangement",                      // 995
   "Ashcroft",                                               // 996
   "Jet Controls Corp",                                      // 997
   "Zumtobel Lighting GmbH",                                 // 998
   "Reserved-by-ASHRAE-999",                                 // 999
   "Ekon GmbH",                                              // 1000
   "Molex",                                                  // 1001
   "Maco Lighting Pty Ltd.",                                 // 1002
   "Axecon Corp.",                                           // 1003
   "Tensor plc",                                             // 1004
   "Kaseman Environmental Control Equipment (Shanghai) Limited", // 1005
   "AB Axis Industries",                                     // 1006
   "Netix Controls",                                         // 1007
   "Eldridge Products, Inc.",                                // 1008
   "Micronics",                                              // 1009
   "Fortecho Solutions Ltd",                                 // 1010
   "Sellers Manufacturing Company",                          // 1011
   "Rite-Hite Doors, Inc.",                                  // 1012
   "Violet Defense LLC",                                     // 1013
   "Simna",                                                  // 1014
   "Multi-Énergie Best Inc.",                                // 1015
   "Mega System Technologies, Inc.",                         // 1016
   "Rheem",                                                  // 1017
   "Ing. Punzenberger COPA-DATA GmbH",                       // 1018
   "MEC Electronics GmbH",                                   // 1019
   "Taco Comfort Solutions",                                 // 1020
   "Alexander Maier GmbH",                                   // 1021
   "Ecorithm, Inc.",                                         // 1022
   "Accurro Ltd",                                            // 1023
   "ROMTECK Australia Pty Ltd",                              // 1024
   "Splash Monitoring Limited",                              // 1025
   "Light Application",                                      // 1026
   "Logical Building Automation",                            // 1027
   "Exilight Oy",                                            // 1028
   "Hager Electro SAS",                                      // 1029
   "KLIF Co., LTD",                                          // 1030
   "HygroMatik",                                             // 1031
   "Daniel Mousseau Programmation & Electronique",           // 1032
   "Aerionics Inc.",                                         // 1033
   "M2S Electronique Ltee",                                  // 1034
   "Automation Components, Inc.",                            // 1035
   "Niobrara Research & Development Corporation",            // 1036
   "Netcom Sicherheitstechnik GmbH",                         // 1037
   "Lumel S.A.",                                             // 1038
   "Great Plains Industries, Inc.",                          // 1039
   "Domotica Labs S.R.L",                                    // 1040
   "Energy Cloud, Inc.",                                     // 1041
   "Vomatec",                                                // 1042
   "Demma Companies",                                        // 1043
   "Valsena",                                                // 1044
   // TODO add more here ...
   // Updated 15 February 2018 from http://www.bacnet.org/VendorID/BACnet%20Vendor%20IDs.htm
   // Use the VTS utility VendorIdTable to update this table.
};
BAC_STRINGTABLE_EX(BACnetVendorID, 0x7FFFFFFF, 0x7FFFFFFF);

STRING_TABLE BACnetVTClass[] = {
   "default-terminal",
   "ansi-x3-64",
   "dec-vt52",
   "dec-vt100",
   "dec-vt220",
   "hp-700-94",
   "ibm-3130"
};
BAC_STRINGTABLE_EX(BACnetVTClass, 64, 65536);

STRING_TABLE BACnetWriteStatus[] = {
   "idle",
   "in-progress",
   "successful",
   "failed"
};
BAC_STRINGTABLE(BACnetWriteStatus);

// Used for bitstring BACnetDaysOfWeek, where Monday == 0
// See also day_of_week
STRING_TABLE BACnetDaysOfWeek[] = {
   "monday",
   "tuesday",
   "wednesday",
   "thursday",
   "friday",
   "saturday",
   "sunday"
};
BAC_STRINGTABLE(BACnetDaysOfWeek);

// Used for Date etc., where Monday == 1
// See also BACnetDaysOfWeek
STRING_TABLE day_of_week[] = {
   "Invalid Day",  /* 0 */
   "Monday",       /* 1 */
   "Tuesday",      /* 2 */
   "Wednesday",    /* 3 */
   "Thursday",     /* 4 */
   "Friday",       /* 5 */
   "Saturday",     /* 6 */
   "Sunday"        /* 7 */
};
BAC_STRINGTABLE(day_of_week);

STRING_TABLE month[] = {
   "Invalid Month", /* 0 */
   "January",       /* 1 */
   "February",      /* 2 */
   "March",         /* 3 */
   "April",         /* 4 */
   "May",           /* 5 */
   "June",          /* 6 */
   "July",          /* 7 */
   "August",        /* 8 */
   "September",     /* 9 */
   "October",       /* 10 */
   "November",      /* 11 */
   "December",      /* 12 */
   "Odd",           /* 13 */
   "Even"           /* 14 */
};
BAC_STRINGTABLE(month);

STRING_TABLE PDU_types[] = {
   "Confirmed Service",
   "Unconfirmed Service",
   "Simple ACK",
   "Complex ACK",
   "Segment ACK",
   "Error",
   "Reject",
   "Abort"
};
BAC_STRINGTABLE(PDU_types);

// madanner 11/12/02, added to support variable stuffing for PDU type,
// values written to var must decode back into values

STRING_TABLE PDU_typesENUM[] = {
   "CONFIRMED-REQUEST",
   "UNCONFIRMED-REQUEST",
   "SIMPLEACK",
   "COMPLEXACK",
   "SEGMENTACK",
   "ERROR",
   "REJECT",
   "ABORT"
};
BAC_STRINGTABLE(PDU_typesENUM);

STRING_TABLE NL_msgs[] = {
   "Who-Is-Router-To-Network",      // 0
   "I-Am-Router-To-Network",
   "I-Could-Be-Router-To-Network",
   "Reject-Message-To-Network",
   "Router-Busy-To-Network",
   "Router-Available-To-Network",
   "Initialize-Routing-Table",
   "Initialize-Routing-Table-Ack",
   "Establish-Connection-To-Network",
   "Disconnect-Connection-To-Network",

   "Challenge-Request",          // 10
   "Security-Payload",
   "Security-Response",
   "Request-Key-Update",
   "Update-Key-Set",
   "Update-Distribution-Key",
   "Request-Master-Key",
   "Set-Master-Key",

   "What-Is-Network-Number",     // 18
   "Network-Number-Is"
};
BAC_STRINGTABLE(NL_msgs);

// This list is also used to create the hash table for scripting.
STRING_TABLE BACnetReject[] = {
   "other",                              /*0*/
   "buffer-overflow",                    /*1*/
   "inconsistent-parameters",            /*2*/
   "invalid-parameter-data-type",        /*3*/
   "invalid-tag",                        /*4*/
   "missing-required-parameter",         /*5*/
   "parameter-out-of-range",             /*6*/
   "too-many-arguments",                 /*7*/
   "undefined-enumeration",              /*8*/
   "unrecognized-service"                /*9*/
};
BAC_STRINGTABLE_EX(BACnetReject, 64, 256);

// This list is also used to create the hash table for scripting.
STRING_TABLE BACnetAbort[] = {
   "other",                             /*0*/
   "buffer-overflow",                   /*1*/
   "invalid-apdu-in-this-state",        /*2*/
   "preempted-by-higher-priority-task", /*3*/
   "segmentation-not-supported"         /*4*/
};
BAC_STRINGTABLE_EX(BACnetAbort, 64, 256);

// Value for ReadPropertyConditionl selectionLogic
STRING_TABLE Selection_Logic[] = {
   "and",
   "or",
   "all"
};
BAC_STRINGTABLE(Selection_Logic);

// Value for ReadPropertyConditionl relationSpecifier
STRING_TABLE Relation_Specifier[] = {
   "Equal (=)",
   "Not Equal (!=)",
   "Less Than (<)",
   "Greater Than (>)",
   "Less Than or Equal (<=)",
   "Greater Than or Equal (>=)"
};
BAC_STRINGTABLE(Relation_Specifier);

STRING_TABLE BVLL_Function[] = {
   "BVLC-Result",
   "Write-Broadcast-Distribution-Table",
   "Read-Broadcast-Distribution-Table",
   "Read-Broadcast-Distribution-Table-Ack",
   "Forwarded-NPDU",
   "Register-Foreign-Device",
   "Read-Foreign-Device-Table",
   "Read-Foreign-Device-Table-Ack",
   "Delete-Foreign-Device-Table-Entry",
   "Distribute-Broadcast-To-Network",
   "Original-Unicast-NPDU",
   "Original-Broadcast-NPDU"
};
BAC_STRINGTABLE(BVLL_Function);

// These have initial upper case for nicer display, since they
// are not actually used as an enumeration.
// This list is used to create the hash table for scripting.
STRING_TABLE BACnetConfirmedServiceChoice[] = {
   "AcknowledgeAlarm",              /* 0 */
   "ConfirmedCOVNotification",      /* 1 */
   "ConfirmedEventNotification",    /* 2 */
   "GetAlarmSummary",               /* 3 */
   "GetEnrollmentSummary",          /* 4 */
   "SubscribeCOV",                  /* 5 */
   "AtomicReadFile",                /* 6 */
   "AtomicWriteFile",               /* 7 */
   "AddListElement",                /* 8 */
   "RemoveListElement",             /* 9 */
   "CreateObject",                  /* 10 */
   "DeleteObject",                  /* 11 */
   "ReadProperty",                  /* 12 */
   "ReadPropertyConditional",       /* 13 */
   "ReadPropertyMultiple",          /* 14 */
   "WriteProperty",                 /* 15 */
   "WritePropertyMultiple",         /* 16 */
   "DeviceCommunicationControl",    /* 17 */
   "ConfirmedPrivateTransfer",      /* 18 */
   "ConfirmedTextMessage",          /* 19 */
   "ReinitializeDevice",            /* 20 */
   "TtOpen",                        /* 21 */
   "VtClose",                       /* 22 */
   "VtData",                        /* 23 */
   "Authenticate",                  /* 24 */
   "RequestKey",                    /* 25 */
   "ReadRange",                     /* 26 */
   "LifeSafetyOperation",           /* 27 */
   "SubscribeCOVProperty",          /* 28 */
   "GetEventInformation"            /* 29 last in 135-2012 (rev 14) */
   // CAUTION: if you add a service here, you must also change max_confirmed_services
   // (which is actually max-plus-one: the NUMBER of defined services)
};                       
BAC_STRINGTABLE(BACnetConfirmedServiceChoice);

STRING_TABLE BACnetUnconfirmedServiceChoice[] = {
   "I-Am",                          /* 0 */
   "I-Have",                        /* 1 */
   "UnconfirmedCOVNotification",    /* 2 */
   "UnconfirmedEventNotification",  /* 3 */
   "UnconfirmedPrivateTransfer",    /* 4 */
   "UnconfirmedTextMessage",        /* 5 */
   "TimeSynchronization",           /* 6 */
   "Who-Has",                       /* 7 */
   "Who-Is",                        /* 8 */
   "UTCTimeSynchronization",        /* 9 */
   "WriteGroup"                     /* 10 last in 135-2012 (rev 14) */
   // CAUTION: if you add a service here, you must also change max_unconfirmed_services
   // (which is actually max-plus-one: the NUMBER of defined services)
};
BAC_STRINGTABLE(BACnetUnconfirmedServiceChoice);

// Not an explicit datatype: defined inline in ReinitializeDevice-Request
STRING_TABLE BACnetReinitializedStateOfDevice[] = {
   "coldstart",   /* 0 */
   "warmstart",
   "startbackup",
   "endbackup",
   "startrestore",
   "endrestore",
   "abortrestore" /* 6 */
};
BAC_STRINGTABLE(BACnetReinitializedStateOfDevice);

// Not an explicit datatype: defined inline in DeviceCommunicationsControl-Request
STRING_TABLE DeviceCommControl_Command[] = {
   "enable",   /* 0 */
   "disable",
   "disable-initiation"
};
BAC_STRINGTABLE(DeviceCommControl_Command);

// Not an explicit datatype: defined inline in ConfirmedTextMessage-Request
STRING_TABLE TextMessage_Priority[] = {
   "normal",   /* 0 */
   "urgent"
};
BAC_STRINGTABLE(TextMessage_Priority);


STRING_TABLE BACnetBackupState[] = {
   "idle",
   "preparing-for-backup",
   "preparing-for-restore",
   "performing-a-backup",
   "performing-a-restore",
   "backup-failure",
   "restore-failure"
};
BAC_STRINGTABLE(BACnetBackupState);

STRING_TABLE NetworkNumberType[] = {
   "learned",
   "configured"
};
BAC_STRINGTABLE(NetworkNumberType);


//=============================================================================
// Empty table to return for undefined enumerations
const static BACnetStringTable s_dummy( NULL, 0 );

//=============================================================================
// Table to map enumeration specifier to a string table for the enumeration.
static const BACnetStringTable* s_enumStringTables[] =
{
   &BAC_STRTAB_FalseTrue,              // 0
   &BAC_STRTAB_BACnetReliability,
   &BAC_STRTAB_BACnetEventState,
   &BAC_STRTAB_BACnetEngineeringUnits,
   &BAC_STRTAB_BACnetNotifyType,
   &BAC_STRTAB_BACnetBinaryPV,
   &BAC_STRTAB_BACnetPolarity,
   &BAC_STRTAB_BACnetDeviceStatus,
   &BAC_STRTAB_BACnetSegmentation,
   &BAC_STRTAB_BACnetEventType,
   &BAC_STRTAB_BACnetFileAccessMethod, // 10
   &BAC_STRTAB_BACnetAction,
   &BAC_STRTAB_BACnetProgramError,
   &BAC_STRTAB_BACnetProgramState,
   &BAC_STRTAB_BACnetProgramRequest,
   &BAC_STRTAB_BACnetObjectType,
   &BAC_STRTAB_BACnetStatusFlags,
   &BAC_STRTAB_BACnetEventTransitionBits,
   &BAC_STRTAB_BACnetLimitEnable,
   &BAC_STRTAB_BACnetVTClass,
   &BAC_STRTAB_BACnetDaysOfWeek,       // 20
   &BAC_STRTAB_NoYes,
   &BAC_STRTAB_month,
   &BAC_STRTAB_BACnetBinaryPV_orNull,
   &BAC_STRTAB_BACnetLifeSafetyState,
   &BAC_STRTAB_BACnetSilencedState,
   &BAC_STRTAB_BACnetLifeSafetyOperation,
   &BAC_STRTAB_BACnetMaintenance,
   &BAC_STRTAB_BACnetLifeSafetyMode,
   &BAC_STRTAB_BACnetAccumulatorStatus,
   &BAC_STRTAB_BACnetLoggingType,      // 30
   &BAC_STRTAB_BACnetShedState,
   &BAC_STRTAB_BACnetNodeType,
   &BAC_STRTAB_BACnetDoorValue,
   &BAC_STRTAB_BACnetDoorStatus,
   &BAC_STRTAB_BACnetLockStatus,
   &BAC_STRTAB_BACnetDoorSecuredStatus,
   &BAC_STRTAB_BACnetDoorAlarmState,
   &BAC_STRTAB_BACnetDoorValue_orNull,
   &BAC_STRTAB_BACnetBackupState,
   &BAC_STRTAB_BACnetRestartReason     // 40
};

//=============================================================================
/// Return a pointer to the BACnetStringTable corresponding to the specified enumeration.
const BACnetStringTable* GetEnumStringTable(int iTableIndex)
{
   const BACnetStringTable *pRetval = &s_dummy;
   if (iTableIndex < sizeof(s_enumStringTables)/sizeof(s_enumStringTables[0]))
   {
      pRetval = s_enumStringTables[iTableIndex];
   }

   return pRetval;
}

} // end namespace NetworkSniffer
