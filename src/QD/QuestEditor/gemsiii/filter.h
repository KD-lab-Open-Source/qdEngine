typedef	unsigned char	Pixel;

#ifdef __cplusplus
extern "C" {
#endif __cplusplus

void resample(Pixel*image_in,int dx_in,int dy_in,
			  Pixel* image_out, int dx_out,int dy_out, char c);


#ifdef __cplusplus
};
#endif __cplusplus
