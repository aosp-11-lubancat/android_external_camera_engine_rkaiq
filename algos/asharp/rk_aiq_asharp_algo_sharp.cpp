
#include "rk_aiq_asharp_algo_sharp.h"

RKAIQ_BEGIN_DECLARE

AsharpResult_t init_sharp_params_v1(RKAsharp_Sharp_HW_Params_t *pParams, CalibDb_Sharp_t *pCalibdb)
{
	AsharpResult_t res = ASHARP_RET_SUCCESS;
	int i = 0;
	int j = 0;
	int max_iso_step = MAX_ISO_STEP;
	
	if(pParams == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(pCalibdb == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	for (i=0;i<max_iso_step;i++){
		pParams->lratio[i] = pCalibdb->sharp_iso[i].lratio;
		pParams->hratio[i] = pCalibdb->sharp_iso[i].hratio;
		pParams->M_ratio[i] = pCalibdb->sharp_iso[i].mf_sharp_ratio;
		pParams->H_ratio[i] = pCalibdb->sharp_iso[i].hf_sharp_ratio;
	}

	for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
		pParams->luma_point[j] = pCalibdb->luma_point[j];
	}

	for (i=0;i<max_iso_step;i++){
		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
            pParams->luma_sigma[i][j] = pCalibdb->sharp_iso[i].luma_sigma[j];
        }		
		pParams->pbf_gain[i] = pCalibdb->sharp_iso[i].pbf_gain;
		pParams->pbf_ratio[i] = pCalibdb->sharp_iso[i].pbf_ratio;
		pParams->pbf_add[i] = pCalibdb->sharp_iso[i].pbf_add;

		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_clp_m[i][j] = pCalibdb->sharp_iso[i].mf_clip_pos[j];
		}	
			
		for(int j = 0; j < RK_EDGEFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_min_m[i][j] = pCalibdb->sharp_iso[i].mf_clip_neg[j];
		}

		for(int j = 0; j < RK_SHARPFILTER_LUMA_POINT_NUM; j++){
			pParams->lum_clp_h[i][j] = pCalibdb->sharp_iso[i].hf_clip[j];
		}
		
		pParams->mbf_gain[i] = pCalibdb->sharp_iso[i].mbf_gain;
		pParams->hbf_gain[i] = pCalibdb->sharp_iso[i].hbf_gain;
		pParams->hbf_ratio[i] = pCalibdb->sharp_iso[i].hbf_ratio;
		pParams->mbf_add[i] = pCalibdb->sharp_iso[i].mbf_add;
		pParams->hbf_add[i] = pCalibdb->sharp_iso[i].hbf_add;
        pParams->ehf_th[i] = pCalibdb->sharp_iso[i].local_sharp_strength;       
    }

	//////////////////////////////////////////////////////////////////////////
	// init filter params
	// Gaussian filter params
	float gaus_luma_coeff[RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM] = 
	{
		0.0625, 0.125, 0.0625,
		0.125, 0.25, 0.125,
		0.0625, 0.125, 0.0625
	};
	// pre bf params
	float pbf_coeff[RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM] =
	{
        0.156250, 0.25, 0.156250, 
        0.25, 0.375000, 0.25, 
        0.156250, 0.25, 0.156250
	};
	// mf range filter params
	float rf_m_coeff[RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM] =
    {
        0.023438, 0.031250, 0.039063, 0.031250, 0.023438, 
        0.031250, 0.046875, 0.054688, 0.046875, 0.031250, 
        0.039063, 0.054688, 0.093750, 0.054688, 0.039063, 
        0.031250, 0.046875, 0.054688, 0.046875, 0.031250,
        0.023438, 0.031250, 0.039063, 0.031250, 0.023438 

    };
	// mf bf params
	float mbf_coeff[RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X] =
    {
        0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.328125, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.406250, 0.0, 0.359375, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.359375, 0.0, 0.406250, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.296875, 0.0, 0.234375, 0.0, 0.234375, 0.0, 0.296875, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.406250, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.171875, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.406250, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.234375, 0.0, 0.140625, 0.109375, 0.140625, 0.0, 0.234375, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.437500, 0.0, 0.0, 0.328125, 0.0, 0.0, 0.171875, 0.109375, 0.0, 0.109375, 0.171875, 0.0, 0.0, 0.328125, 0.0, 0.0, 0.437500, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.234375, 0.0, 0.140625, 0.109375, 0.140625, 0.0, 0.234375, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.406250, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.171875, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.406250, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.296875, 0.0, 0.234375, 0.0, 0.234375, 0.0, 0.296875, 0.0, 0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.406250, 0.0, 0.359375, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.359375, 0.0, 0.406250, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.343750, 0.0, 0.328125, 0.0, 0.343750, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.406250, 0.0, 0.0, 0.0, 0.0  
    };
	// hf range filter params
	float rf_h_coeff[RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM] =
    {
        0.0, 0.015625, 0.023438, 0.015625, 0.0, 
        0.015625, 0.062500, 0.101563, 0.062500, 0.015625, 
        0.023438, 0.101563, 0.125000, 0.101563, 0.023438, 
        0.015625, 0.062500, 0.101563, 0.062500, 0.015625,
		0.0, 0.015625, 0.023438, 0.015625, 0.0
    };
	// hf bf params
	float hbf_coeff[RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM] = 
    {
        0.156250, 0.25, 0.156250, 
        0.25, 0.375000, 0.25, 
        0.156250, 0.25, 0.156250 
    };

	for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_GAU_DIAM;
        int w = RKSHAPRENHW_GAU_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++){
                pParams->gaus_luma_kernel[i][m * w + n] = gaus_luma_coeff[m * w + n];
			}
        }
    }

    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_PBF_DIAM;
        int w = RKSHAPRENHW_PBF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_pbf[i][m * w + n] = pbf_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_MRF_DIAM;
        int w = RKSHAPRENHW_MRF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->h_rf_m[i][m * w + n] = rf_m_coeff[m * w + n];
        }
    }

	for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_MBF_DIAM_Y;
        int w = RKSHAPRENHW_MBF_DIAM_X;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_mbf[i][m * w + n] = mbf_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_HRF_DIAM;
        int w = RKSHAPRENHW_HRF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->h_rf_h[i][m * w + n] = rf_h_coeff[m * w + n];
        }
    }
	
    for (i=0; i<max_iso_step; i++){
        int h = RKSHAPRENHW_HBF_DIAM;
        int w = RKSHAPRENHW_HBF_DIAM;
        for(int m = 0; m < h; m++){
            for(int n = 0; n < w; n++)
                pParams->kernel_hbf[i][m * w + n] = hbf_coeff[m * w + n];
        }
    }

	return res;
	
}

AsharpResult_t select_rk_sharpen_hw_params_by_ISO(				
	RKAsharp_Sharp_HW_Params_t *strksharpenParams, 	
	RKAsharp_Sharp_HW_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
{
    int i;
	int iso_low, iso_high;
	int gain_high, gain_low;
	float ratio;
	int iso_div 			= 50;
    int max_iso_step        = MAX_ISO_STEP;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(strksharpenParams == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(strksharpenParamsSelected == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}
	
	for (i = max_iso_step - 1; i >= 0; i--)
	{
		if (iso < iso_div * (2 << i))
		{
			iso_low = iso_div * (2 << (i)) / 2;
			iso_high = iso_div * (2 << i);
		}
	}
	ratio = (float)(iso - iso_low)/(iso_high-iso_low);
	if (iso_low == iso)
	{
		iso_high = iso;
		ratio = 0;
	}
	if (iso_high == iso )
	{
		iso_low = iso;
	    	ratio = 1;
	}
	gain_high 		= (int)(log((float)iso_high / 50) /log((float)2));
	gain_low 		= (int)(log((float)iso_low / 50) / log((float)2));


	gain_low		= MIN(MAX(gain_low, 0), 8);
	gain_high		= MIN(MAX(gain_high, 0), 8);



    strksharpenParamsSelected->lratio               = INTERP1(strksharpenParams->lratio     [gain_low],     strksharpenParams->lratio       [gain_high],    ratio);
    strksharpenParamsSelected->hratio               = INTERP1(strksharpenParams->hratio     [gain_low],     strksharpenParams->hratio       [gain_high],    ratio);
    strksharpenParamsSelected->M_ratio              = INTERP1(strksharpenParams->M_ratio    [gain_low],     strksharpenParams->M_ratio      [gain_high],    ratio);
    strksharpenParamsSelected->H_ratio              = INTERP1(strksharpenParams->H_ratio    [gain_low],     strksharpenParams->H_ratio      [gain_high],    ratio);
	strksharpenParamsSelected->pbf_ratio            = INTERP1(strksharpenParams->pbf_ratio  [gain_low],     strksharpenParams->pbf_ratio    [gain_high],    ratio);
    strksharpenParamsSelected->hbf_ratio            = INTERP1(strksharpenParams->hbf_ratio  [gain_low],     strksharpenParams->hbf_ratio    [gain_high],    ratio);
    strksharpenParamsSelected->ehf_th               = (short)ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
    for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
    {
        strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
        strksharpenParamsSelected->luma_sigma[i]    = INTERP1(strksharpenParams->luma_sigma [gain_low][i],  strksharpenParams->luma_sigma   [gain_high][i], ratio);
        
        strksharpenParamsSelected->lum_clp_m[i]     = (short)ROUND_F(INTERP1(strksharpenParams->lum_clp_m  [gain_low][i],  strksharpenParams->lum_clp_m    [gain_high][i], ratio));
        strksharpenParamsSelected->lum_min_m[i]     = INTERP1(strksharpenParams->lum_min_m  [gain_low][i],  strksharpenParams->lum_min_m    [gain_high][i], ratio);

        strksharpenParamsSelected->lum_clp_h[i]     = (short)ROUND_F(INTERP1(strksharpenParams->lum_clp_h  [gain_low][i],  strksharpenParams->lum_clp_h    [gain_high][i], ratio));
    }
	strksharpenParamsSelected->pbf_gain      = INTERP1(strksharpenParams->pbf_gain   [gain_low],  strksharpenParams->pbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->pbf_add       = INTERP1(strksharpenParams->pbf_add    [gain_low],  strksharpenParams->pbf_add      [gain_high], ratio);

	strksharpenParamsSelected->mbf_gain      = INTERP1(strksharpenParams->mbf_gain   [gain_low],  strksharpenParams->mbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->mbf_add       = INTERP1(strksharpenParams->mbf_add    [gain_low],  strksharpenParams->mbf_add      [gain_high], ratio);

	strksharpenParamsSelected->hbf_gain      = INTERP1(strksharpenParams->hbf_gain   [gain_low],  strksharpenParams->hbf_gain     [gain_high], ratio);
	strksharpenParamsSelected->hbf_add       = INTERP1(strksharpenParams->hbf_add    [gain_low],  strksharpenParams->hbf_add      [gain_high], ratio);

    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->gaus_luma_kernel) / sizeof(strksharpenParamsSelected->gaus_luma_kernel[0]); i++)
        strksharpenParamsSelected->gaus_luma_kernel[i]    = INTERP1(strksharpenParams->gaus_luma_kernel [gain_low][i],  strksharpenParams->gaus_luma_kernel   [gain_high][i], ratio);
    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->kernel_pbf) / sizeof(strksharpenParamsSelected->kernel_pbf[0]); i++)
        strksharpenParamsSelected->kernel_pbf[i]    = INTERP1(strksharpenParams->kernel_pbf [gain_low][i],  strksharpenParams->kernel_pbf   [gain_high][i], ratio);
    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->h_rf_m) / sizeof(strksharpenParamsSelected->h_rf_m[0]); i++)
        strksharpenParamsSelected->h_rf_m[i]        = INTERP1(strksharpenParams->h_rf_m [gain_low][i],      strksharpenParams->h_rf_m       [gain_high][i], ratio);
    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->kernel_mbf) / sizeof(strksharpenParamsSelected->kernel_mbf[0]); i++)
        strksharpenParamsSelected->kernel_mbf[i]    = INTERP1(strksharpenParams->kernel_mbf [gain_low][i],  strksharpenParams->kernel_mbf   [gain_high][i], ratio);
    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->h_rf_h) / sizeof(strksharpenParamsSelected->h_rf_h[0]); i++)
        strksharpenParamsSelected->h_rf_h[i]        = INTERP1(strksharpenParams->h_rf_h [gain_low][i],      strksharpenParams->h_rf_h       [gain_high][i], ratio);
    for(unsigned int i = 0;i < sizeof(strksharpenParamsSelected->kernel_hbf) / sizeof(strksharpenParamsSelected->kernel_hbf[0]); i++)
        strksharpenParamsSelected->kernel_hbf[i]    = INTERP1(strksharpenParams->kernel_hbf [gain_low][i],  strksharpenParams->kernel_hbf   [gain_high][i], ratio);

    return res;
}

AsharpResult_t select_rk_sharpen_hw_v2_params_by_ISO(
	RKAsharp_Sharp_HW_V2_Params_t *strksharpenParams, 
	RKAsharp_Sharp_HW_V2_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
{
	int i;
	int iso_low, iso_high;
	int gain_high, gain_low;
	float ratio;
	int iso_div 			= 50;
	int max_iso_step        = MAX_ISO_STEP;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(strksharpenParams == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(strksharpenParamsSelected == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}
	
	for (i = max_iso_step - 1; i >= 0; i--)
	{
		if (iso < iso_div * (2 << i))
		{
			iso_low = iso_div * (2 << (i)) / 2;
			iso_high = iso_div * (2 << i);
		}
	}
	ratio = (float)(iso - iso_low)/(iso_high-iso_low);
	if (iso_low == iso)
	{
		iso_high = iso;
		ratio = 0;
	}
	if (iso_high == iso )
	{
		iso_low = iso;
		ratio = 1;
	}
	gain_high 		= (int)(log((float)iso_high / 50) /log((float)2));
	gain_low 		= (int)(log((float)iso_low / 50) / log((float)2));

	gain_low		= MIN(MAX(gain_low, 0), 8);
	gain_high		= MIN(MAX(gain_high, 0), 8);

	strksharpenParamsSelected->pbf_gain				= INTERP1(strksharpenParams->pbf_gain	[gain_low],		strksharpenParams->pbf_gain 	[gain_high], 	ratio);
	strksharpenParamsSelected->pbf_add				= INTERP1(strksharpenParams->pbf_add	[gain_low],		strksharpenParams->pbf_add		[gain_high], 	ratio);
	strksharpenParamsSelected->pbf_ratio            = INTERP1(strksharpenParams->pbf_ratio  [gain_low],     strksharpenParams->pbf_ratio    [gain_high],    ratio);
	strksharpenParamsSelected->lratio               = INTERP1(strksharpenParams->lratio     [gain_low],     strksharpenParams->lratio       [gain_high],    ratio);
	strksharpenParamsSelected->hratio               = INTERP1(strksharpenParams->hratio     [gain_low],     strksharpenParams->hratio       [gain_high],    ratio);
	strksharpenParamsSelected->sharp_ratio          = INTERP1(strksharpenParams->sharp_ratio[gain_low],     strksharpenParams->sharp_ratio  [gain_high],    ratio);	
	strksharpenParamsSelected->hbf_gain      		= INTERP1(strksharpenParams->hbf_gain   [gain_low],  	strksharpenParams->hbf_gain     [gain_high], 	ratio);
	strksharpenParamsSelected->hbf_add       		= INTERP1(strksharpenParams->hbf_add    [gain_low],  	strksharpenParams->hbf_add      [gain_high], 	ratio);
	strksharpenParamsSelected->hbf_ratio            = INTERP1(strksharpenParams->hbf_ratio  [gain_low],     strksharpenParams->hbf_ratio    [gain_high],    ratio);
	strksharpenParamsSelected->ehf_th               = (short)ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
	for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
	{
		strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
		strksharpenParamsSelected->luma_sigma[i]    = INTERP1(strksharpenParams->luma_sigma [gain_low][i],  strksharpenParams->luma_sigma   [gain_high][i], ratio);		
		strksharpenParamsSelected->lum_clip_h[i]    = (short)ROUND_F(INTERP1(strksharpenParams->lum_clip_h  [gain_low][i],  strksharpenParams->lum_clip_h    [gain_high][i], ratio));
	}
	for(int i = 0;i < RKSHAPRENHW_V2_GAU_DIAM * RKSHAPRENHW_V2_GAU_DIAM; i++)
	{
		strksharpenParamsSelected->gaus_luma_kernel[i] = INTERP1(strksharpenParams->gaus_luma_kernel [gain_low][i],  strksharpenParams->gaus_luma_kernel[gain_high][i], ratio);
	}

	for(int i = 0;i < RKSHAPRENHW_V2_PBF_DIAM * RKSHAPRENHW_V2_PBF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_pre_bf[i] = INTERP1(strksharpenParams->kernel_pre_bf [gain_low][i], strksharpenParams->kernel_pre_bf[gain_high][i], ratio);
	}
	for(int i = 0;i < RKSHAPRENHW_V2_HRF_DIAM * RKSHAPRENHW_V2_HRF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_range_filter[i] = INTERP1(strksharpenParams->kernel_range_filter[gain_low][i], strksharpenParams->kernel_range_filter[gain_high][i], ratio);
	}
	for(int i = 0;i < RKSHAPRENHW_V2_HBF_DIAM_Y * RKSHAPRENHW_V2_HBF_DIAM_X; i++)
	{
		strksharpenParamsSelected->kernel_hf_bf[i] = INTERP1(strksharpenParams->kernel_hf_bf[gain_low][i],  strksharpenParams->kernel_hf_bf[gain_high][i], ratio);
	}

	return res;
}

AsharpResult_t select_rk_sharpen_hw_v3_params_by_ISO(
	RKAsharp_Sharp_HW_V3_Params_t *strksharpenParams, 
	RKAsharp_Sharp_HW_V3_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
{
	int i;
	int iso_low, iso_high;
	int gain_high, gain_low;
	float ratio;
	int iso_div 			= 50;
	int max_iso_step        = MAX_ISO_STEP;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(strksharpenParams == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(strksharpenParamsSelected == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}
	
	for (i = max_iso_step - 1; i >= 0; i--)
	{
		if (iso < iso_div * (2 << i))
		{
			iso_low = iso_div * (2 << (i)) / 2;
			iso_high = iso_div * (2 << i);
		}
	}
	ratio = (float)(iso - iso_low)/(iso_high-iso_low);
	if (iso_low == iso)
	{
		iso_high = iso;
		ratio = 0;
	}
	if (iso_high == iso )
	{
		iso_low = iso;
		ratio = 1;
	}
	gain_high 		= (int)(log((float)iso_high / 50) /log((float)2));
	gain_low 		= (int)(log((float)iso_low / 50) / log((float)2));

	gain_low		= MIN(MAX(gain_low, 0), 8);
	gain_high		= MIN(MAX(gain_high, 0), 8);

	strksharpenParamsSelected->lratio				= INTERP1(strksharpenParams->lratio		[gain_low],		strksharpenParams->lratio 		[gain_high], 	ratio);
	strksharpenParamsSelected->hratio				= INTERP1(strksharpenParams->hratio		[gain_low],		strksharpenParams->hratio		[gain_high], 	ratio);
	strksharpenParamsSelected->sharp_ratio          = INTERP1(strksharpenParams->sharp_ratio[gain_low],     strksharpenParams->sharp_ratio  [gain_high],    ratio);	
	strksharpenParamsSelected->ehf_th               = (short)ROUND_F(INTERP1(strksharpenParams->ehf_th     [gain_low],     strksharpenParams->ehf_th       [gain_high],    ratio));
	for(int i = 0; i < RK_EDGEFILTER_LUMA_POINT_NUM; i++)
	{
		strksharpenParamsSelected->luma_point[i]    = strksharpenParams->luma_point[i];
		strksharpenParamsSelected->lum_clip_h[i]    = (short)ROUND_F(INTERP1(strksharpenParams->lum_clip_h  [gain_low][i],  strksharpenParams->lum_clip_h    [gain_high][i], ratio));
	}
	for(int i = 0;i < RKSHAPRENHW_V3_RF_DIAM * RKSHAPRENHW_V3_RF_DIAM; i++)
	{
		strksharpenParamsSelected->kernel_range_filter[i] = INTERP1(strksharpenParams->kernel_range_filter [gain_low][i], strksharpenParams->kernel_range_filter[gain_high][i], ratio);
	}

	return res;
}

void select_sharpen_params_by_ISO	(
	RKAsharp_Sharp_Params_t *strksharpenParams,		
	RKAsharp_Sharp_Params_Select_t *strksharpenParamsSelected, 
	int iso
)
{
	select_rk_sharpen_hw_params_by_ISO(&strksharpenParams->rk_sharpen_params_V1, &strksharpenParamsSelected->rk_sharpen_params_selected_V1, iso);
	
	select_rk_sharpen_hw_v2_params_by_ISO(&strksharpenParams->rk_sharpen_params_V2, &strksharpenParamsSelected->rk_sharpen_params_selected_V2, iso);
	
	select_rk_sharpen_hw_v3_params_by_ISO(&strksharpenParams->rk_sharpen_params_V3, &strksharpenParamsSelected->rk_sharpen_params_selected_V3, iso);
}



AsharpResult_t rk_Sharp_V1_fix_transfer(RKAsharp_Sharp_HW_Params_Select_t *pSharpV1, RKAsharp_Sharp_HW_Fix_t* pSharpCfg)
{
	int i = 0;
	int k = 0;
	int tmp = 0;
	int sum_coeff, offset;
	float sum_coeff_float;
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(pSharpV1 == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(pSharpCfg == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}
	
	
	//0x0080
	pSharpCfg->yin_flt_en = 0;
	pSharpCfg->edge_avg_en = 0;
	
	//0x0084
	pSharpCfg->hbf_ratio = (unsigned short)ROUND_F(pSharpV1->hbf_ratio * (1 << reg_sharpenHW_hbf_ratio_fix_bits)); 
	pSharpCfg->ehf_th = (unsigned short)ROUND_F(pSharpV1->ehf_th); 
	pSharpCfg->pbf_ratio = (unsigned short)ROUND_F(pSharpV1->pbf_ratio * (1 << reg_sharpenHW_pbf_ratio_fix_bits));


	//0x0090
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->pbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[4] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[1] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
	pSharpCfg->pbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_pbf[0] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));	
	for(k = 0; k <RKSHAPRENHW_PBF_DIAM * RKSHAPRENHW_PBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_pbf[k] * (1 << reg_sharpenHW_pPBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_pbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pPBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->pbf_k[0] = pSharpCfg->pbf_k[0] + offset;
	

	//0x0094 - 0x0098
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[12] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[7] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[6] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[2] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[1] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	pSharpCfg->mrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_m[0] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MRF_DIAM * RKSHAPRENHW_MRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_m[k] * (1 << reg_sharpenHW_pMRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_m[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mrf_k[0] = pSharpCfg->mrf_k[0] + offset;

	//0x009c -0x00a4
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->mbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[110] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[93] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[92] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[3] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[76] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[4] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[58] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[5] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[56] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[6] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[25] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[7] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[23] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[8] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[38] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[9] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[4] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[10] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[69] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	pSharpCfg->mbf_k[11] = (unsigned char)ROUND_F(pSharpV1->kernel_mbf[102] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
	for(k = 0; k <RKSHAPRENHW_MBF_DIAM_Y * RKSHAPRENHW_MBF_DIAM_X; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_mbf[k] * (1 << reg_sharpenHW_pMBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_mbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pMBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->mbf_k[0] = pSharpCfg->mbf_k[0] + offset;
	
	//0x00a8 -0x00ac
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hrf_k[0] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[12] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[1] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[7] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[2] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[6] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[3] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[2] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[4] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[1] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	pSharpCfg->hrf_k[5] = (unsigned char)ROUND_F(pSharpV1->h_rf_h[0] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HRF_DIAM * RKSHAPRENHW_HRF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->h_rf_h[k] * (1 << reg_sharpenHW_pHRfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->h_rf_h[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHRfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hrf_k[0] = pSharpCfg->hrf_k[0] + offset;

	//0x00b0
	sum_coeff = 0;
	sum_coeff_float = 0;
	pSharpCfg->hbf_k[0] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[4] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[1] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[1] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	pSharpCfg->hbf_k[2] = (unsigned char)ROUND_F(pSharpV1->kernel_hbf[0] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
	for(k = 0; k < RKSHAPRENHW_HBF_DIAM * RKSHAPRENHW_HBF_DIAM; k ++)
	{
		sum_coeff += ROUND_F(pSharpV1->kernel_hbf[k] * (1 << reg_sharpenHW_pHBfCoeff_fix_bits));
		sum_coeff_float += pSharpV1->kernel_hbf[k];
	}
	offset = int(sum_coeff_float * (1 << reg_sharpenHW_pHBfCoeff_fix_bits)) - sum_coeff;
	pSharpCfg->hbf_k[0] = pSharpCfg->hbf_k[0] + offset;
	
	//0x00b4
	
	//0x00b8

	//0x00bc - 0x00c0
	
	//0x00c4 - 0x00c8

	//0x00cc - 0x00d0
	for(i=0; i<6; i++){
		pSharpCfg->lum_point[i] = (unsigned char)ROUND_F(pSharpV1->luma_point[i+1]);
	}
	
	//0x00d4
	//pbf
	int sigma_deci_bits = 7;
	int sigma_inte_bits = 1;
	int max_val			= 0;
	int min_val			= 65536;
	int shf_bits		= 0;
	short sigma_bits[3];
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->pbf_shf_bits = sigma_bits[2] - 5;

	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] 
						* pSharpV1->mbf_gain 
						+ pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->mbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);

	// hf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < 8; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];
	pSharpCfg->hbf_shf_bits = (unsigned char)(sigma_bits[2] - 5);
	

	//0x00d8 - 0x00dc
	// pre bf sigma inv
	sigma_deci_bits = 7;
	sigma_inte_bits = 1;
	max_val			= 0;
	min_val			= 65536;
	shf_bits		= 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR((pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add));
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->pbf_sigma[i] = (unsigned char)ROUND_F((float)1 
								/ (pSharpV1->luma_sigma[i] * pSharpV1->pbf_gain + pSharpV1->pbf_add)  
								* (1 << sigma_bits[2]));
	}

	//0x00e0 - 0x00e4
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_m[i] = (unsigned char)(pSharpV1->lum_clp_m[i]);
	}

	//0x00e8 - 0x00ec
	for(i=0; i<8; i++){
		pSharpCfg->lum_min_m[i] = (char)ROUND_F(pSharpV1->lum_min_m[i] * (1 << reg_sharpenHW_lum_min_m_fix_bits));
	}

	//0x00f0 - 0x00f4
	// mf bf sigma inv
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->mbf_sigma[i] = (unsigned char)ROUND_F((float)1
								/ (pSharpV1->luma_sigma[i] * pSharpV1->mbf_gain + pSharpV1->mbf_add) 
								* (1 << sigma_bits[2]));
	}

	//0x00f8 - 0x00fc
	for(i=0; i<8; i++){
		pSharpCfg->lum_clp_h[i] = (unsigned char)(pSharpV1->lum_clp_h[i]);
	}

	//0x0100 - 0x0104
	//hbf
	max_val = 0;
	min_val = 65536;
	shf_bits = 0;
	for(int i = 0; i < RK_SHARPFILTER_LUMA_POINT_NUM; i++)
	{
		int cur_sigma = FLOOR(pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add);
		if(max_val < cur_sigma)
			max_val = cur_sigma;
		if(min_val > cur_sigma)
			min_val = cur_sigma;
	}
	sigma_bits[0] = FLOOR(log((float)min_val)/log((float)2));
	sigma_bits[1] = MAX(sigma_inte_bits - sigma_bits[0], 0);
	sigma_bits[2] = sigma_deci_bits + sigma_bits[0];

	for(i=0; i<8; i++){
		pSharpCfg->hbf_sigma[i] = (unsigned char)ROUND_F((float)1 
								/ (pSharpV1->luma_sigma[i] * pSharpV1->hbf_gain + pSharpV1->hbf_add) 
								* (1 << sigma_bits[2]));
	}

	//0x0128
	pSharpCfg->rfl_ratio = (unsigned short)ROUND_F(pSharpV1->lratio * (1 << reg_sharpenHW_lratio_fix_bits));
	pSharpCfg->rfh_ratio = (unsigned short)ROUND_F(pSharpV1->hratio * (1 << reg_sharpenHW_hratio_fix_bits)); 

	//0x012C
	pSharpCfg->m_ratio = (unsigned char)ROUND_F(pSharpV1->M_ratio * (1 << reg_sharpenHW_M_ratio_fix_bits)); 
	pSharpCfg->h_ratio = (unsigned char)ROUND_F(pSharpV1->H_ratio * (1 << reg_sharpenHW_H_ratio_fix_bits));


	return res;
}

AsharpResult_t rk_Sharp_fix_transfer(RKAsharp_Sharp_Params_Select_t* sharp, RKAsharp_Sharp_Fix_t* pSharpCfg)
{
	AsharpResult_t res = ASHARP_RET_SUCCESS;

	if(sharp == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}

	if(pSharpCfg == NULL){
		LOGE_ASHARP("%s(%d): null pointer\n", __FUNCTION__, __LINE__);
		return ASHARP_RET_NULL_POINTER;
	}
	
	rk_Sharp_V1_fix_transfer(&sharp->rk_sharpen_params_selected_V1, &pSharpCfg->stSharpFixV1);

	return res;
}


RKAIQ_END_DECLARE
