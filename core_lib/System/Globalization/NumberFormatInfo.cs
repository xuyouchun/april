/*
 *  System.dll
 *
 *  NumberFormatInfo: the symbols that the text of a number has (the signs, the separators, the currency).
 */

using Reflection;

namespace Globalization;

////////// ////////// ////////// ////////// //////////

// The symbols of the text of a number. There are no cultures: CurrentInfo has the symbols of the United States (the point,
// the comma, $), InvariantInfo has the same but the currency symbol (¤). Both are read only: Clone() makes an info that is
// changed, and it is given to the numbers (n.ToString("N2", info)) and to String.Format(info, format, args).
public class NumberFormatInfo : IFormatProvider
{
    public NumberFormatInfo()
    {
        __negativeSign = "-";
        __decimalSeparator = ".";
        __groupSeparator = ",";
        __percentSymbol = "%";
        __currencySymbol = "$";
        __nanSymbol = "NaN";
        __positiveInfinitySymbol = "Infinity";
        __negativeInfinitySymbol = "-Infinity";
        __numberDecimalDigits = 2;
        __percentDecimalDigits = 2;
        __currencyDecimalDigits = 2;
    }

    // The info of the United States, read only.
    public static NumberFormatInfo CurrentInfo
    {
        get
        {
            if (__current == null)
            {
                NumberFormatInfo info = new NumberFormatInfo();
                info.__readOnly = true;
                __current = info;
            }

            return __current;
        }
    }

    // The info that does not depend on a culture, read only; its currency symbol is ¤.
    public static NumberFormatInfo InvariantInfo
    {
        get
        {
            if (__invariant == null)
            {
                NumberFormatInfo info = new NumberFormatInfo();
                info.__currencySymbol = "¤";
                info.__readOnly = true;
                __invariant = info;
            }

            return __invariant;
        }
    }

    // The info of a provider: the provider is an info, or it gives one (GetFormat), or it is null (CurrentInfo).
    public static NumberFormatInfo GetInstance(IFormatProvider? provider)
    {
        if (provider == null)
            return CurrentInfo;

        NumberFormatInfo? info = provider as NumberFormatInfo;
        if (info != null)
            return info;

        info = provider.GetFormat(typeof(NumberFormatInfo)) as NumberFormatInfo;
        return info == null? CurrentInfo : info;
    }

    // A copy that can be changed.
    public NumberFormatInfo Clone()
    {
        NumberFormatInfo info = new NumberFormatInfo();
        info.__negativeSign = __negativeSign;
        info.__decimalSeparator = __decimalSeparator;
        info.__groupSeparator = __groupSeparator;
        info.__percentSymbol = __percentSymbol;
        info.__currencySymbol = __currencySymbol;
        info.__nanSymbol = __nanSymbol;
        info.__positiveInfinitySymbol = __positiveInfinitySymbol;
        info.__negativeInfinitySymbol = __negativeInfinitySymbol;
        info.__numberDecimalDigits = __numberDecimalDigits;
        info.__percentDecimalDigits = __percentDecimalDigits;
        info.__currencyDecimalDigits = __currencyDecimalDigits;

        return info;
    }

    public Boolean IsReadOnly => __readOnly;

    // Returns the info itself for the type NumberFormatInfo.
    public Object? GetFormat(Type? formatType)
    {
        if (formatType != null && formatType.Equals(typeof(NumberFormatInfo)))
            return this;

        return null;
    }

    public String NegativeSign
    {
        get => __negativeSign;
        set { __CheckWritable(); __negativeSign = __NotNull(value); }
    }

    public String NumberDecimalSeparator
    {
        get => __decimalSeparator;
        set { __CheckWritable(); __decimalSeparator = __NotNull(value); }
    }

    public String NumberGroupSeparator
    {
        get => __groupSeparator;
        set { __CheckWritable(); __groupSeparator = __NotNull(value); }
    }

    public String PercentSymbol
    {
        get => __percentSymbol;
        set { __CheckWritable(); __percentSymbol = __NotNull(value); }
    }

    public String CurrencySymbol
    {
        get => __currencySymbol;
        set { __CheckWritable(); __currencySymbol = __NotNull(value); }
    }

    public String NaNSymbol
    {
        get => __nanSymbol;
        set { __CheckWritable(); __nanSymbol = __NotNull(value); }
    }

    public String PositiveInfinitySymbol
    {
        get => __positiveInfinitySymbol;
        set { __CheckWritable(); __positiveInfinitySymbol = __NotNull(value); }
    }

    public String NegativeInfinitySymbol
    {
        get => __negativeInfinitySymbol;
        set { __CheckWritable(); __negativeInfinitySymbol = __NotNull(value); }
    }

    // The digits after the point of F, N (NumberDecimalDigits), P (PercentDecimalDigits) and C (CurrencyDecimalDigits)
    // when the format has no number of digits.
    public Int32 NumberDecimalDigits
    {
        get => __numberDecimalDigits;
        set { __CheckWritable(); __numberDecimalDigits = __CheckDigits(value); }
    }

    public Int32 PercentDecimalDigits
    {
        get => __percentDecimalDigits;
        set { __CheckWritable(); __percentDecimalDigits = __CheckDigits(value); }
    }

    public Int32 CurrencyDecimalDigits
    {
        get => __currencyDecimalDigits;
        set { __CheckWritable(); __currencyDecimalDigits = __CheckDigits(value); }
    }

    private Void __CheckWritable()
    {
        if (__readOnly)
            throw new InvalidOperationException("the info is read only: Clone() makes one that can be changed");
    }

    private static String __NotNull(String? value)
    {
        if (value == null)
            throw new ArgumentException("the symbol is null");

        return value;
    }

    private static Int32 __CheckDigits(Int32 value)
    {
        if (value < 0 || value > 99)
            throw new OutOfRangeException();

        return value;
    }

    private String __negativeSign;

    private String __decimalSeparator;

    private String __groupSeparator;

    private String __percentSymbol;

    private String __currencySymbol;

    private String __nanSymbol;

    private String __positiveInfinitySymbol;

    private String __negativeInfinitySymbol;

    private Int32 __numberDecimalDigits;

    private Int32 __percentDecimalDigits;

    private Int32 __currencyDecimalDigits;

    private Boolean __readOnly;

    private static NumberFormatInfo? __current;

    private static NumberFormatInfo? __invariant;
}
