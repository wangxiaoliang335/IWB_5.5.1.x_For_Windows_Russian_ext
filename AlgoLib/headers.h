#pragma once
#include <atlstr.h>
#include <limits>
#include <fstream>
#include <assert.h>
//#include <pathcch.h>
//#pragma comment(lib, "Pathcch.lib")

#include "SIMD.h"
#include <mmintrin.h>
#include "MyAPI.h"
#include "../inc/VideoProcAmpProperty.h"
#include "../inc/MultiDimensionPoint.h"
#include "../inc/PerfDetector.h"
#include "../inc/ImageFrame.h"
#include "../inc/DIB/Dib.h"
#include "../inc/BitFrame.h"
#include "../inc/MJPG/MJPG_Encoder.h"
#include "../inc/ImageJPEG.h"
#include "../inc/ImageDebug.h"
#include "../inc/DispMonitorFinder.h"
#include "AutoCalibratorImpl2.h"
#include "./BlobDetector/BlobDetectorImpl.h"
#include "./Calibrator/CalibrateAlgo.h"
#include "./ScreenAreaDetector/ScreenAreaDetectorImpl.h"
#include "../MorphologyAlgo/inc/MorphologyAlgo.h"
#include "../inc/AlgoLib.h"
#include "GlobalVar.h"
#include "SIMD.h"
#include "FillPolygon.h"
#include "../png/pngapi.h"

//#include "SortInDirection.h"



