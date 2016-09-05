using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace pCarsAPI_Demo
{
    public class lapTimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            float _tmp = (float)value;

            if (_tmp < 0.0001)
            {
                return "_";
            }
            else
            {
                return (TimeSpan.FromSeconds(_tmp).ToString(@"mm\:ss\.fff"));
            }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }

    public class sectTimeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            decimal _tmp;
            decimal.TryParse(value.ToString(), out _tmp);

            if (_tmp == null)
                return null;

            TimeSpan _tsTemp = new TimeSpan((long)(_tmp * 10000000));

            if (_tmp <= 0)
            {
                return ".";
            }
            else
            {
                if (_tsTemp.Minutes > 0)
                {
                    return (_tsTemp.ToString(@"m\:ss\.fff"));
                }
                else
                {
                    return (_tsTemp.ToString(@"s\.fff"));
                }
            }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }

    public class zeroDigitFloatAllowZeroConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            decimal _tmp;
            decimal.TryParse(value.ToString(), out _tmp);

            if (_tmp == null)
                return null;

            return (_tmp.ToString("N0"));
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }

    public class twoDigitFloatAllowZeroConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            decimal _tmp;
            decimal.TryParse(value.ToString(), out _tmp);

            if (_tmp == null)
                return null;

            return (_tmp.ToString("N2"));
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }

    public class gearConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if ((int)value < 0)
            {
                return "r";
            }
            else
            {
                if ((int)value == 0)
                {
                    return "n";
                }
                else
                {
                    return value;
                }
            }
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }

    }
}
