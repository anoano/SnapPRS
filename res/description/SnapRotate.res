CONTAINER SnapRotate
{
  NAME SnapRotate;
	GROUP ID_MAIN_ATR
	{
		DEFAULT 1;
		GROUP
		{
			COLUMNS 2;
			GROUP
			{
				COLUMNS 2;
				LONG MDATA_SELECTMODE
				{
					CYCLE
					{
						CH_ROT;
						CH_EXTUBE;
					}
				}
				BITMAPBUTTON 	MDATA_SAVES {}
			}
		//	STATICTEXT { DUMMY; }
			REAL ID_ROT_X { UNIT DEGREE; }
			BUTTON IDB_BTN_SET{}
			LONG IDE_MODE
			{
				CYCLE
				{
					CH_3SNAP;
					CH_2SNAP;
				}
			}
			STATICTEXT { DUMMY; }
			LONG MDATA_LIST
			{
				CYCLE
				{
					CH_WORLD;
					CH_LOCAL;
				}
			}	
			REAL ROT_ANGLE_VALUE {UNIT DEGREE; MIN 0; }
			LONG MDATA_EXTUBESUBDEV { MIN 1; STEP 1;}
			STATICTEXT { DUMMY; }
			SEPARATOR { LINE; }
			STATICTEXT { DUMMY; }
			BUTTON MDATA_CROSSPOSBTN{}
			BUTTON MDATA_CROSSRESETBTN {}
			BOOL MDATA_SW2D{}
		}
		SEPARATOR { LINE; }
	}
	INCLUDE SnapTransfer;
}
