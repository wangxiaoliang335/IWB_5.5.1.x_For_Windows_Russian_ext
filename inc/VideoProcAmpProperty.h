#pragma once
//video propertise on a video capture device.
struct TVideoProcAmpProperty
{
public:

	long Prop_VideoProcAmp_Brightness           ;//亮度
	long Prop_VideoProcAmp_Contrast             ;//对比度
	long Prop_VideoProcAmp_Hue                  ;//色调
	long Prop_VideoProcAmp_Satuation            ;//饱和度
	long Prop_VideoProcAmp_Sharpness            ;//锐利度
	long Prop_VideoProcAmp_Gamma                ;//Gamma值
	long Prop_VideoProcAmp_ColorEnable          ;//颜色启用
	long Prop_VideoProcAmp_WhiteBalance         ;//白平衡
	long Prop_VideoProcMap_BacklightCompensation;//背光补偿
	long Prop_VideoProcMap_Gain                 ;//增益

	long Prop_CameraControl_Exposure            ;//曝光时间
	
	//TVideoProcAmpProperty()
	//{
	//	Prop_VideoProcAmp_Brightness            = 125;
	//	Prop_VideoProcAmp_Contrast              = 50;
	//	Prop_VideoProcAmp_Hue                   = 0;
	//	Prop_VideoProcAmp_Satuation             = 104;
	//	Prop_VideoProcAmp_Sharpness             = 8;
	//	Prop_VideoProcAmp_Gamma                 = 46;
	//	Prop_VideoProcAmp_ColorEnable           = 0;
	//	Prop_VideoProcAmp_WhiteBalance          = 5000;
	//	Prop_VideoProcMap_BacklightCompensation = 2;
	//	Prop_VideoProcMap_Gain                  = 0;
	//}

};