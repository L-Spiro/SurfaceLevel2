/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A floating-point value with any number of bits, separated into a sign bit, X exponent bits, and Y mantissa bits.
 */


#pragma once

#include <cfloat>
#include <cmath>
#include <cstdint>

#define SL2_FLOATX_DBL_EXP_BITS			11
#define SL2_FLOATX_DBL_MAN_BITS			DBL_MANT_DIG
#define SL2_FLOATX_FLT_EXP_BITS			8
#define SL2_FLOATX_FLT_MAN_BITS			FLT_MANT_DIG
#define SL2_FLOAT16_EPSILON				0.0009765625f

namespace sl2 {

	/**
	 * Class CFloatX
	 * \brief A floating-point value with any number of bits, separated into a sign bit, X exponent bits, and Y mantissa bits.
	 *
	 * Description: A floating-point value with any number of bits, separated into a sign bit, X exponent bits, and Y mantissa bits.
	 */
	class CFloatX {
	public :
		CFloatX() :
			uiExponent( 0 ),
			uiMantissa( 0 ),
			uiManBits( 0 ),
			uiExpBits( 0 ),
			bSign( false ),
			bImplicitManBit( true ),
			bHasSign( false ) {
		}
		uint64_t						uiExponent;
		uint64_t						uiMantissa;
		uint16_t						uiExpBits;
		uint16_t						uiManBits;
		bool							bSign;
		bool							bImplicitManBit;
		bool							bHasSign;


		// == Functions.
		/**
		 * Creates a +InF value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateInfP( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates a -InF value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateInfN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates a NaN value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateNaN( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates a max value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateMax( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates the smallest normalized value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateMinNormalized( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates the smallest non-0 value.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateMin( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates epsilon, the smallest value such that 1.0+X does not equal 1.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateEpsilon( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates the next-after value going up from the given number.
		 * 
		 * \param _dVal The reference value.
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateNextAfterUp( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Creates the next-after value going down from the given number.
		 * 
		 * \param _dVal The reference value.
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateNextAfterDown( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign );

		/**
		 * Is this a NaN?
		 *
		 * \return Returns true if this value represents the Not-a-Number of the current bit configuration for the float.
		 **/
		bool							IsNaN() const;

		/**
		 * Is this +InF?
		 *
		 * \return Returns true if this value represents the Positive-Infinity of the current bit configuration for the float.
		 **/
		bool							IsInfP() const;

		/**
		 * Is this -InF?
		 *
		 * \return Returns true if this value represents the Negative-Infinity of the current bit configuration for the float.
		 **/
		bool							IsInfN() const;

		/**
		 * Create from a double.
		 * 
		 * \param _dVal The reference value.
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateFromDouble( double _dVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * Create from bits in a uint64_t value.
		 * 
		 * \param _uiVal The reference value.
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateFromBits( uint64_t _uiVal, uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * Create from separate values.
		 * 
		 * \param _uiSign The sign value.
		 * \param _uiExp The exponent value.
		 * \param _uiMan The mantissa value.
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						CreateFromParts( uint64_t _uiSign, uint64_t _uiExp, uint64_t _uiMan,
			uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit = true, bool _bHasSign = true );

		/**
		 * Cast to double.
		 *
		 * \return Returns the value as a 64-bit double.
		 **/
		double							AsDouble() const;

		/**
		 * Put all the bits together into a uint64_t value.
		 *
		 * \return Returns the encoded bits of the float.
		 **/
		uint64_t						AsUint64() const;

		/**
		 * Gets the exponent with all bits set.
		 *
		 * \return Returns true if all exponents bits are set.
		 **/
		uint64_t						AllExpBitsSet() const { return AllExpBitsSet( uiExpBits ); }

		/**
		 * Put all the bits together into a uint64_t value except for the sign value.
		 *
		 * \return Returns the combined value.
		 **/
		uint64_t						AsUint64SansSign() const;

		/**
		 * Gets the exponent bias.
		 *
		 * \return Returns the exponent bias.
		 **/
		double							ExpBias() const { return ExpBias( uiExpBits ); }

		/**
		 * Gets the denormalized exponent bias.
		 *
		 * \return Returns the denormalized exponent bias.
		 **/
		double							DenormalExpBias() const { return ExpBias() - 1; }

		/**
		 * Determines if the type is denormalized or not.
		 *
		 * \return Returns true if the value is denormalized.
		 **/
		bool							IsDenormalized() const { return uiExponent == 0; }

		/**
		 * Gets a sign multiplier.
		 *
		 * \return Returns the sign as -1.0 or 1.0.
		 **/
		double							Sign() const { return ((bHasSign && bSign) ? -1.0 : 1.0); }

		/**
		 * Gets the sign bit shifted.
		 *
		 * \return Gets the sign bit as a 1 or 0.
		 **/
		uint64_t						SignBit() const { return ((bHasSign && bSign) ? 1 : 0); }

		/**
		 * Gets the exponent bits shifted.
		 *
		 * \return Returns the shifted exponent bits.
		 **/
		uint64_t						ExpBits() const { return uiExponent & ((1ULL << uiExpBits) - 1ULL); }

		/**
		 * Gets the mantissa bits shifted.
		 *
		 * \return Returns the shifted mantissa bits.
		 **/
		uint64_t						ManBits() const { return uiMantissa & ((1ULL << RealMantissaBits( uiManBits, bImplicitManBit )) - 1ULL); }

		/**
		 * Sets the sign bit, if applicable.
		 *
		 * \param _bEnabled Whether the sign bit should be set or unset.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						SetSign( bool _bEnabled );

		/**
		 * Sets or unsets a bit in the exponent.
		 *
		 * \param _bEnabled To set or unset the given bit.
		 * \param _uiBit The bit to set or unset.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						SetExpBit( bool _bEnabled, uint16_t _uiBit );

		/**
		 * Sets or unsets a bit in the mantissa.
		 *
		 * \param _bEnabled To set or unset the given bit.
		 * \param _uiBit The bit to set or unset.
		 * \return Returns this object after performing the operation.
		 **/
		CFloatX &						SetManBit( bool _bEnabled, uint16_t _uiBit );

		/**
		 * Gets the maximum possible value for a float type with the given bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \return Returns the maximum possible value for the given configuration.
		 **/
		static double					GetMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * Gets the smallest possible non-0 value for a float type with the given bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \return Returns the maximum possible value for the given configuration.
		 **/
		static double					GetMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * Gets the smallest normalized non-0 value for a float type with the given bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \return Returns the maximum possible value for the given configuration.
		 **/
		static double					GetNormalizedMinForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * Gets the maximum denormalized value for a float type with the given bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \return Returns the maximum possible value for the given configuration.
		 **/
		static double					GetDenormalizedMaxForBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit );

		/**
		 * Gets an exponent with all bits set.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \return Returns an exponent with all bits set.
		 **/
		static uint64_t					AllExpBitsSet( uint16_t _uiExpBits ) { return (1ULL << _uiExpBits) - 1; }

		/**
		 * Gets the exponent bias.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \return Returns the exponent bias.
		 **/
		static const double				ExpBias( uint16_t _uiExpBits ) { return double( (1ULL << (_uiExpBits - 1)) - 1 ); }

		/**
		 * Gets the maximum normalized exponent bias for a given number of bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \return Returns the maximum normalized exponent bias for a given number of bits.
		 **/
		static const double				MaxExpBias( uint16_t _uiExpBits ) { return double( (1ULL << (_uiExpBits - 1)) - 1 ); }

		/**
		 * Gets the minimum normalized exponent bias for a given number of bits.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \return Returns the minimum normalized exponent bias for a given number of bits.
		 **/
		static const double				MinExpBias( uint16_t _uiExpBits ) { return double( (1ULL << (_uiExpBits - 1)) - 1 ); }

		/**
		 * Gets the maximum number of bits in a sign.
		 *
		 * \return Returns the maximum sign bits.
		 **/
		static constexpr uint32_t		MaxSignBits() { return 1; }

		/**
		 * Gets the maximum number of bits in an exponent.
		 *
		 * \return Returns the maximum number of bits in an exponent.
		 **/
		static constexpr uint32_t		MaxExpBits() { return 11; }

		/**
		 * Gets the maximum number of bits in a mantissa.
		 *
		 * \return Returns the maximum number of bits in a mantissa.
		 **/
		static constexpr uint32_t		MaxManBits() { return SL2_FLOATX_DBL_MAN_BITS; }

		/**
		 * Gets the real number of mantissa bits based on whether or not there is an implicit bit.
		 *
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \return Returns the real number of mantissa bits based on whether or not there is an implicit bit.
		 **/
		static const uint64_t			RealMantissaBits( uint16_t _uiManBits, bool _bImplicitMantissaBit ) { return _bImplicitMantissaBit ? _uiManBits - 1 : _uiManBits; }

		/**
		 * Gets the sign bit from a double (not shifted).
		 * 
		 * \param _dVal The value from which to extract the sign bit.
		 * \return Returns the sign bit of the given value.
		 **/
		static uint64_t					SignBit( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (1ULL << 63ULL); }

		/**
		 * Gets the exponent from a double (not shifted).
		 * 
		 * \param _dVal The value from which to extract the exponent bits.
		 * \return Returns the exponent bits of the given value.
		 **/
		static uint64_t					ExpBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << 11ULL) - 1ULL) << (SL2_FLOATX_DBL_MAN_BITS - 1ULL)); }

		/**
		 * Gets the mantissa from a double (not shifted).
		 * 
		 * \param _dVal The value from which to extract the mantissa bits.
		 * \return Returns the mantissa from a double (not shifted).
		 **/
		static uint64_t					ManBits( double _dVal ) { return (*reinterpret_cast<uint64_t *>(&_dVal)) & (((1ULL << (SL2_FLOATX_DBL_MAN_BITS - 2ULL)) - 1ULL)); }

		/**
		 * Gets the number of bits in a given representation.
		 * 
		 * \param _uiExpBits Exponent bits.
		 * \param _uiManBits Mantissa bits.
		 * \param _bImplicitMantissaBit Implicit bit.
		 * \param _bHasSign Sign bit.
		 * \return Returns the number of bits in a given representation.
		 **/
		static uint64_t					TotalBits( uint16_t _uiExpBits, uint16_t _uiManBits, bool _bImplicitMantissaBit, bool _bHasSign ) {
			return (_uiExpBits + _uiManBits) + (_bHasSign ? 1ULL : 0ULL) - (_bImplicitMantissaBit ? 1ULL : 0ULL);
		}

		/**
		 * Gets the precision given the number of mantissa bits for numbers greater than 1.
		 * 
		 * \param _uiManBits Mantissa bits.
		 * \return Returns the precision given the number of mantissa bits for numbers greater than 1.
		 **/
		static double					Precision( uint16_t _uiManBits ) {
			return _uiManBits > 1 ? std::log10( std::pow( 2.0, _uiManBits ) ) :
				(std::log10( _uiManBits ) + 1.0);
		}
	};

}	// namespace sl2
