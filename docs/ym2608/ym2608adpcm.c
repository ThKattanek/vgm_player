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

	/* �����l�̐ݒ� */
	xn			= 0;
	stepSize	= 127;
	flag		= 0;
	
	for( lpc = 0 ; lpc < len ; lpc++ )
	{
		/* �G���R�[�h����2 */
		dn = *src - xn;
		src++;

		/*
			�G���R�[�h����3�A4
			I = | dn | / Sn ����An�����߂�B
			�搔���g�p���Đ����ʂŉ��Z����B
		*/
		i = ( abs( dn ) << 16 ) / ( stepSize << 14 );
		if( i > 7 ) i = 7;
		adpcm = ( unsigned char )i;

		/*
			�G���R�[�h����5
			L3+L2/2+L1/4+1/8 * stepSize ��8�{���Đ������Z
		*/
		i = ( adpcm * 2 + 1 ) * stepSize / 8;

		/* 1-2*L4 -> L4��1�̏ꍇ��-1��������̂Ɠ��� */
		if( dn < 0 )
		{
			/*
				-�̏ꍇ�����r�b�g��t����B
				�G���R�[�h����5��ADPCM�������ז��ɂȂ�̂ŁA�\���l�X�V���܂ŕۗ������B
			*/
			adpcm |= 0x8;
			xn -= i;
		}
		else
		{
			xn += i;
		}

		/*
			�G���R�[�h����6
			�X�e�b�v�T�C�Y�̍X�V
		*/
		stepSize = ( stepsizeTable[ adpcm ] * stepSize ) / 64;

		/* �G���R�[�h����7	*/
		if( stepSize < 127 )
			stepSize = 127;
		else if( stepSize > 24576 )
			stepSize = 24576;

		/* ADPCM�ŕۑ����� */
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

	/* �����l�̐ݒ� */
	xn			= 0;
	stepSize	= 127;
	flag		= 0;
	shift		= 4;
	step		= 0;

	for( lpc = 0 ; lpc < len ; lpc++ )
	{
		adpcm = ( *src >> shift ) & 0xf;

		/*
			�f�R�[�h����2�A3
			L3+L2/2+L1/4+1/8 * stepSize ��8�{���Đ������Z
		*/
		i = ( ( adpcm & 7 ) * 2 + 1 ) * stepSize / 8;
		if( adpcm & 8 )
			xn -= i;
		else
			xn += i;

		/* �f�R�[�h����4 */
		if( xn > 32767 )
			xn = 32767;
		else if( xn < -32768 )
			xn = -32768;

		/* �f�R�[�h����5 */
		stepSize = stepSize * stepsizeTable[ adpcm ] / 64;

		/* �f�R�[�h����6 */
		if( stepSize < 127 )
			stepSize = 127;
		else if ( stepSize > 24576 )
			stepSize = 24576;

		/* PCM�ŕۑ����� */
		*dest = ( short )xn;
		dest++;

		src += step;
		step = step ^ 1;
		shift = shift ^ 4;
	}

	return 0;
}
