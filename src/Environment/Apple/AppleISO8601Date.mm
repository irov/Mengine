#import "AppleISO8601Date.h"

@implementation AppleISO8601Date

+ (NSTimeInterval)NSTimeIntervalFromISO8601Duration:(NSString *)date {
    if (date.length == 0) {
        return NAN;
    }

    // Regex breakdown:
    // P                             – prefix
    // (?:(\\d+)Y)?                  – years
    // (?:(\\d+)M)?                  – months (date part)
    // (?:(\\d+)D)?                  – days
    // (?:T                          – time separator
    //     (?:(\\d+)H)?              – hours
    //     (?:(\\d+)M)?              – minutes (time part)
    //     (?:(\\d+(?:\\.\\d+)?)S)?  – seconds
    // )?                            – time part optional
    NSString *pattern =
        @"^P"
         "(?:(\\d+)Y)?"
         "(?:(\\d+)M)?"
         "(?:(\\d+)D)?"
         "(?:T"
             "(?:(\\d+)H)?"
             "(?:(\\d+)M)?"
             "(?:(\\d+(?:\\.\\d+)?)S)?"
         ")?$";

    NSRegularExpression * re =
        [NSRegularExpression regularExpressionWithPattern:pattern
                                                  options:0
                                                    error:nil];

    NSTextCheckingResult * match =
        [re firstMatchInString:date
                       options:0
                         range:NSMakeRange(0, date.length)];

    if (match == nil) {
        return NAN; // Invalid format
    }

    // Helper to convert a captured group into double; returns 0 if empty.
    double (^valueAt)(NSInteger) = ^double (NSInteger idx) {
        NSRange r = [match rangeAtIndex:idx];
        if (r.location == NSNotFound || r.length == 0) {
            return 0.0;
        }
        return [[date substringWithRange:r] doubleValue];
    };

    const double kSecondsPerMinute = 60.0;
    const double kSecondsPerHour   = 60.0 * kSecondsPerMinute;
    const double kSecondsPerDay    = 24.0 * kSecondsPerHour;

    // Approximation constants (change if needed)
    const double kSecondsPerMonth  = 30.0 * kSecondsPerDay;
    const double kSecondsPerYear   = 365.0 * kSecondsPerDay;

    double years   = valueAt(1);
    double months  = valueAt(2);
    double days    = valueAt(3);
    double hours   = valueAt(4);
    double minutes = valueAt(5);
    double seconds = valueAt(6);

    return  years   * kSecondsPerYear   +
            months  * kSecondsPerMonth  +
            days    * kSecondsPerDay    +
            hours   * kSecondsPerHour   +
            minutes * kSecondsPerMinute +
            seconds;
}

@end
