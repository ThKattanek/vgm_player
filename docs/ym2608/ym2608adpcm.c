/*
	YM2608 ADPCM Codec

	code : Masashi Wada ( DEARNA )
	http://www.memb.jp/~dearna/
*/

#include <math.h>

static long stepsizeTable[ 16 ] =
{
	57, 57, 57, 57, 77,102,128,153,
	57, 57, 57, 57, 77,102,128,153
};

int YM2608ADPCM_Encode( short *src , unsigned char *dest , int len )
{
	int lpc , flag;
	long i , dn , xn , stepSize;
	unsigned char adpcm;
	unsigned char adpcmPack;

	/* 初期値の設定 */
	xn			= 0;
	stepSize	= 127;
	flag		= 0;
	
	for( lpc = 0 ; lpc < len ; lpc++ )
	{
		/* エンコード処理2 */
		dn = *src - xn;
		src++;

		/*
			エンコード処理3、4
			I = | dn | / Sn からAnを求める。
			乗数を使用して整数位で演算する。
		*/
		i = ( abs( dn ) << 16 ) / ( stepSize << 14 );
		if( i > 7 ) i = 7;
		adpcm = ( unsigned char )i;

		/*
			エンコード処理5
			L3+L2/2+L1/4+1/8 * stepSize を8倍して整数演算
		*/
		i = ( adpcm * 2 + 1 ) * stepSize / 8;

		/* 1-2*L4 -> L4が1の場合は-1をかけるのと同じ */
		if( dn < 0 )
		{
			/*
				-の場合符号ビットを付ける。
				エンコード処理5でADPCM符号が邪魔になるので、予測値更新時まで保留した。
			*/
			adpcm |= 0x8;
			xn -= i;
		}
		else
		{
			xn += i;
		}

		/*
			エンコード処理6
			ステップサイズの更新
		*/
		stepSize = ( stepsizeTable[ adpcm ] * stepSize ) / 64;

		/* エンコード処理7	*/
		if( stepSize < 127 )
			stepSize = 127;
		else if( stepSize > 24576 )
			stepSize = 24576;

		/* ADPCMで保存する */
		if( flag == 0 )
		{
			adpcmPack = ( adpcm << 4 ) ;
			flag = 1;
		}
		else
		{
			adpcmPack |= adpcm;
			*dest = adpcmPack;
			dest++;
			flag = 0;
		}
	}

	return 0;
}

int YM2608ADPCM_Decode( unsigned char *src , short *dest , int len )
{
	int lpc , flag , shift , step;
	long i , xn , stepSize;
	long adpcm;

	/* 初期値の設定 */
	xn			= 0;
	stepSize	= 127;
	flag		= 0;
	shift		= 4;
	step		= 0;

	for( lpc = 0 ; lpc < len ; lpc++ )
	{
		adpcm = ( *src >> shift ) & 0xf;

		/*
			デコード処理2、3
			L3+L2/2+L1/4+1/8 * stepSize を8倍して整数演算
		*/
		i = ( ( adpcm & 7 ) * 2 + 1 ) * stepSize / 8;
		if( adpcm & 8 )
			xn -= i;
		else
			xn += i;

		/* デコード処理4 */
		if( xn > 32767 )
			xn = 32767;
		else if( xn < -32768 )
			xn = -32768;

		/* デコード処理5 */
		stepSize = stepSize * stepsizeTable[ adpcm ] / 64;

		/* デコード処理6 */
		if( stepSize < 127 )
			stepSize = 127;
		else if ( stepSize > 24576 )
			stepSize = 24576;

		/* PCMで保存する */
		*dest = ( short )xn;
		dest++;

		src += step;
		step = step ^ 1;
		shift = shift ^ 4;
	}

	return 0;
}
