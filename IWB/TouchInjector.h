#pragma once
    class TouchInjector
    {
    public:

        TouchInjector()
        {
            InitializeTouchInjection(MAX_TOUCH_CONTACT_NUMER, TOUCH_FEEDBACK_DEFAULT);

        }


        void Input(const TContactInfo* pPenInfos, int nPenCount)
        {
          std::vector<POINTER_TOUCH_INFO> vecPointTouchInfo;

            vecPointTouchInfo.resize(nPenCount);

            for (int i = 0; i < nPenCount; i++)
            {
                const TContactInfo& contactInfo = pPenInfos[i];

                POINTER_TOUCH_INFO& pointer_touch_info = vecPointTouchInfo[i];
                memset(&pointer_touch_info, 0, sizeof(pointer_touch_info));

                pointer_touch_info.pointerInfo.pointerType = PT_TOUCH; //we're sending touch input
                pointer_touch_info.pointerInfo.pointerId = contactInfo.uId;//contact Id
                pointer_touch_info.pointerInfo.ptPixelLocation.x = contactInfo.pt.x;
                pointer_touch_info.pointerInfo.ptPixelLocation.y = contactInfo.pt.y;

                POINTER_FLAGS pointerFlags = POINTER_FLAG_NONE;

                auto itFound = m_mapOldContactInfos.find(contactInfo.uId);
                bool bAlreadyExist = (itFound == m_mapOldContactInfos.end()) ? false : true;

                if (bAlreadyExist)
                {
                    if (contactInfo.ePenState == E_PEN_STATE_UP)
                    {
                        pointerFlags = POINTER_FLAG_UP;

                        //弹起了, 从历史记录中删除。
                        m_mapOldContactInfos.erase(itFound);
                    }
                    else
                    {
                        //更新坐标位置
                        pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
                        itFound->second = contactInfo;
                    }
                    

                }
                else
                {
                    if (contactInfo.ePenState == E_PEN_STATE_DOWN)
                    {
                        pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
                    }
                    m_mapOldContactInfos[contactInfo.uId] = contactInfo;
                }

                pointer_touch_info.pointerInfo.pointerFlags = pointerFlags;
                pointer_touch_info.touchFlags = TOUCH_FLAG_NONE;
                pointer_touch_info.touchMask = TOUCH_MASK_NONE;// TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
                pointer_touch_info.orientation = 90;
                pointer_touch_info.pressure = 32000;

                //
                // set the contact area depending on thickness
                long x = contactInfo.pt.x;
                long y = contactInfo.pt.y;

                pointer_touch_info.rcContact.top    = y - 2;
                pointer_touch_info.rcContact.bottom = y + 2;
                pointer_touch_info.rcContact.left   = x - 2;
                pointer_touch_info.rcContact.right  = x + 2;
            }

            if (nPenCount)
            {
                InjectTouchInput(nPenCount, &vecPointTouchInfo[0]);
            }

        }

        //从pointerId到TContactInfo的映射
        std::map<UINT, TContactInfo> m_mapOldContactInfos;

        static const int MAX_TOUCH_CONTACT_NUMER = 30;
    };

