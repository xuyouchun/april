# Time (System, System.Diagnostics)

The library reads two clocks of the system, by two internal functions (`Time_UtcNowTicks`, `Time_MonotonicTicks`, in
`rtlib/system.cpp`: the interpreter, the JIT and the native programs have them). A tick is 100 nanoseconds, as in .NET.

| Type | What it is |
|---|---|
| `TimeSpan` | a length of time, in ticks (`Int64`): `Zero`, `MinValue`, `MaxValue`; the constructors (`hours, minutes, seconds`, `days, ...`, `ticks`); `Days Hours Minutes Seconds Milliseconds`, `TotalDays ... TotalMilliseconds`; `FromDays ... FromMilliseconds` (a `Double`, the part of a tick is cut), `FromTicks`; `Add Subtract Negate Duration Multiply Divide`; the operators `+ - * /` and the comparisons; `ToString()` (`[-][d.]hh:mm:ss[.fffffff]`). A result that does not fit is an `OverflowException`, a `NaN` an `ArgumentException`. There is no `Parse`. |
| `DateTime` | a point of time from 0001-01-01 to 9999-12-31 in the Gregorian calendar, in ticks. **All times are UTC**: there are no time zones, no local time, no `Now`, no `Kind`. `UtcNow`, `Today`, `UnixEpoch`; the constructors (`year, month, day [, hour, minute, second [, millisecond]]`, `ticks`); `Year ... Millisecond`, `DayOfWeek`, `DayOfYear`, `Date`, `TimeOfDay`; `Add*`, `AddMonths` (the day is cut to the last day of the month), `AddYears`, `Subtract`; the operators; `IsLeapYear`, `DaysInMonth`; `ToString()` (`MM/dd/yyyy HH:mm:ss`) and `ToString(format)` for `G d T s O`. A date that does not exist is an `OutOfRangeException`. There is no `Parse`, no custom format. |
| `DayOfWeek` | the enum of .NET (`Sunday` is 0). The text of an enum value is the name of the type for now: print `(Int32)day`. |
| `Stopwatch` (`System.Diagnostics`) | measures with the clock that only goes on: `Start Stop Reset Restart`, `IsRunning`, `ElapsedTicks`, `ElapsedMilliseconds`, `Elapsed`, `StartNew()`, `GetTimestamp()`, `Frequency` (10,000,000), `IsHighResolution`. |
| `Environment` | `TickCount64` (milliseconds of the clock that only goes on; its zero is not defined), `TickCount` (an `Int32`), `NewLine`. |

`DateTime.UtcNow` is the clock of the system, that can be set: a time can be before one that was read earlier. For measuring,
use `Stopwatch`, or the difference of two `Stopwatch.GetTimestamp()`.

The tests are in `test/unittest/core_lib/System/` (`timespan.cs`, `datetime.cs`) and `.../Diagnostics/stopwatch.cs`; what depends on
the time of the run is told as true or false (a time is not less than the one before), not printed.
