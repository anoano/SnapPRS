#include "SnapAlignTool.h"


////ベクトル表示
//void printcheck( String st,Vector p)
//{
//	GePrint(st+".x="+String::FloatToString(p.x)+st+".y="+String::FloatToString(p.y)+st+".z="+String::FloatToString(p.z));
//
/////////////////////////////////////////////
SnapAlign::SnapAlign()
{

	//EndMg=Matrix();
	NextFlag=0;
	G_markpoint=Vector(0,0,0);
	G_SnapFlag=false;
	G_P1=Vector();

	UV_Sw=false;
	Mp1=Vector();
	Drag=false;
	majer=0;

	PlaneSet_BTNFlag=false;

	PlaneSetCnt=0;

	PlV[0]=PlV[1]=PlV[2]=Vector();

	PopFlag=false;
	MenuDrag=-1;
	MenuV1=Vector();
	Odlg_flag=0;

	CutSnapFlag=0;


	HUDOffset=40;
	static Bool Titleflag=false;

	InitFlag= false;
	if( !Titleflag )
	{
	
		GePrint("--------------------------------------------------------");
		GePrint(GeLoadString(IDS_SNAPPRS) + GeLoadString(IDC_VER));
		GePrint("copyright(c) Since 2011 CoffeeStock All rights reserved.");
		GePrint("--------------------------------------------------------");
		Titleflag=true;
	}
	
	_snap = nullptr;
}

SnapAlign::~SnapAlign()
{
	ResetTool();
	if (_snap)
		SnapCore::Free(_snap);
	_snap = nullptr;
}

Bool SnapAlign::GetCursorInfo(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, Float x, Float y, BaseContainer &bc)
{
	if (bc.GetId()==BFM_CURSORINFO_REMOVE) return true;
	bc.SetString(RESULT_BUBBLEHELP, GeLoadString(IDS_HLP_SNAPTRANSFER));

	if (bc.GetId() == BFM_CURSORINFO_REMOVE)
		return true;


	_MX=x;
	_MY=y;
	DTOOL DP;

	//***********************************************
	//スナップ設定ダイアログを閉じた場合のスナップ同期設定
	if (SnpDialogFlag)
	{
		if (!DP.SnapSync(doc, data, PlaneSet_BTNFlag)) return false;
		EventAdd();
		SnpDialogFlag = false;
	}
	//***********************************************
	//SnapTest
	//// allocate snap if necessary
	if (!_snap)
	{
		_snap = SnapCore::Alloc();
		if (!_snap) return false;
	}

	// initialize snap always
	if (!_snap->Init(doc, bd))
		return false;


	Int32 view = doc->GetActiveBaseDraw()->GetProjection();




	
	//キー取得
	BaseContainer state;
	GetInputState(BFM_INPUT_KEYBOARD, BFM_INPUT_QUALIFIER, state);
	Int32 qual = state.GetInt32(BFM_INPUT_QUALIFIER);
	AltKey=false;
	PShift=false;



	//作業平面設定と基準マトリックスの取得
	if( NextFlag==0)
	{
		EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg, 0);
	}
	
	
	if(qual == QALT) //ALTキー押した場合はカーソルロック
	{
		AltKey=true; //ALTキー判定フラグ
		LocV=G_markpoint; //マウス座標を記憶
	}
	//CTRLキー	押した場合は軸モード
	else if(qual== QCTRL)
	{
		MemoPos = G_markpoint;
		EdgeID = -1;


		if(!CtrlOn)
		{
			AxisPos=bd->SW(Vector(x,y,500.0));//押した時のマウス座標を取得
			CtrlOn = true; //Axisi描画フラグ
		}
		//各軸とマウス座標の重なり判定
		if(DP.FuzzyPointInRange(data, bd, AxisPos,x,y)) //軸中心のチェック
		{
			EdgeID=30;
		}
		else
		{
			BaseDraw *bbd = doc->GetActiveBaseDraw();
			view =	bbd->GetProjection();
			//ライン末端とマウス座標の重なり判定処理
			//有効の場合はオフセットダイアログが有効
			if (view == 6 || view == 7) //XZ平面
			{
				if (DP.FuzzyPointInRange(data, bd, $VectorXYZ[2], x, y))
				{
					EdgeID = 170;
				}
			}
			else if (view == 4 || view == 5) //XY平面
			{
				if (DP.FuzzyPointInRange(data, bd, $VectorXYZ[0], x, y))
				{
					EdgeID = 150;
				}
			}
			else if (view == 2 || view == 3) //YZ平面
			{
				if (DP.FuzzyPointInRange(data, bd, $VectorXYZ[1], x, y))
				{
					EdgeID = 160;
				}
			}
			else{
				for (Int32 k = 0; k<3; k++)
				{
					if (DP.FuzzyPointInRange(data, bd, $VectorXYZ[k], x, y))
					{
						if (k == 0) EdgeID = 150;
						else if (k == 1) EdgeID = 160;
						else EdgeID = 170;
					}
				}
			}
			//ラインとマウス座標の重なり判定処理
			if (EdgeID == -1)
			{
				if (!DP.PointerToAXISEdgeCheck(bd, data, x, y, Aline, AxisPos, &EdgeID))
				{
					EdgeID = -1;
				}
			}

			if(view==6 && EdgeID==0)
			{
				EdgeID=4;
			}
            //オルソビューの場合は鉛直方向軸は無効
			if(view==2 || view==3)
			{
				data.SetBool(MDATA_SNAP_AX ,false);
			}
			if(view==4 || view==5)
			{
				data.SetBool(MDATA_SNAP_AZ ,false);
			}
			if(view==6 || view==7)
			{
				data.SetBool(MDATA_SNAP_AY ,false);
			}
		}

		if(data.GetBool(MDATA_BOOL_OFFSET))
		{
			if(DP.FuzzyPointInRange(data, bd, lineV[0],x,y) && data.GetBool(MDATA_SNAP_AX))
			{ 
				EdgeID = 100;
			}
			if(DP.FuzzyPointInRange(data, bd, lineV[1],x,y)&& data.GetBool(MDATA_SNAP_AY))
			{
				EdgeID = 200;
			}
			if(DP.FuzzyPointInRange(data, bd, lineV[2],x,y) && data.GetBool(MDATA_SNAP_AZ))
			{
				EdgeID = 300;
			}
		}
		MMP=bd->SW(Vector(x,y,500));
	}
	else
	{
		Vector G_nvec = Vector(0, 1, 0);
		_Starget1 = DP.SnapMouseVectorObj(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, 0, G_P1, G_P1, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY,  &SnapID1, G_markpoint  );

		if(qual == QSHIFT) //Shiftキー押した場合
		{
			PShift=true; //shiftキーの判定フラグ
		}	

		CtrlOn=false;

		if(MenuOpen1==1)
		{
			Sndlg.Close(false);
			MenuOpen1=0;
		}
		if(MenuOpen1==4)
		{
			dlg.Close(false);
			MenuOpen1=0;
		}
		if(Odlg_flag)
		{
			Odlg.Close(false);
			Odlg_flag=0;
		}
		//分割スナップモード
		if (CutSnapFlag)
		{
			MpRX[0] = x;
			MpRX[1] = y;

			Int32 cutCnt = data.GetInt32(SNAP_SECTION); //分割数取得
			//長さを求める
			Vector unit = (SPANV[0] - SPANV[1]) / cutCnt;
			Vector fv = SPANV[1];
			for (Int32 i = 0; i< cutCnt + 1; i++)
			{
				Vector pv = fv + unit*i;
				if (DP.FuzzyPointInRange(data, bd, pv, x, y))
				{
					G_markpoint = pv;
					CutSnapFlag = 2;
					G_SnapFlag = 1;
					MMP = G_markpoint;
					//	printcheck("G_markpoint=", G_markpoint);
					break;
				}
				else CutSnapFlag = 1;
			}
		}

		if (G_SnapFlag == 0)
		{
			MMP = G_markpoint; //マウスカーソルの座標
		}
	}

	//Vector r = MatrixToHPB(EndMg, ROTATIONORDER_DEFAULT);
	//DP.printcheckR("EndMg Rot=", r);
	//r = MatrixToHPB(FaceMg, ROTATIONORDER_DEFAULT);
	//DP.printcheckR("FaceMg Rot=", r);
	//GePrint("G_SnapFlag=" + String::IntToString(G_SnapFlag));
	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
	
	return true;
}

//初期化
void SnapAlign::InitDefaultSettings(BaseDocument *doc, BaseContainer &data)
{

//**************************************************
	DTOOL DP;
	DP.init(doc, data);
	Priinit( data, false );
//**************************************************
	ResetTool();
	if (!DP.SnapSync(doc, data, PlaneSet_BTNFlag)) return;
	DescriptionToolData::InitDefaultSettings(doc,data);

}

//--------------------------------------------------------------------------------------------------------------------------
//初期化
Bool SnapAlign::InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt)
{
	DTOOL DP;
    SYSTEMINFO    stype =        GeGetSystemInfo();
	if ( stype & (SYSTEMINFO_SAVABLEDEMO|SYSTEMINFO_SAVABLEDEMO_ACTIVE) || stype & SYSTEMINFO_COMMANDLINE  )
	{
        
	}
	else
	{
		//シリアルチェック
		
		if(!DP.KeyCoodCheck( 1024956 )) return false;
	}

	ResetTool();

		//ツールのスナップ設定をメインスナップ設定に反映
	if (!DP.SnapSync(doc, data, PlaneSet_BTNFlag)) return false;

	InitFlag= true;
	//メインスナップ設定をツールスナップ設定に反映
	//if (!DP.CheckSnapSetting(doc, data)) return false;

	return true;
}

void SnapAlign::ResetTool()
{
	
	//plane= GetWorkplaneObject( doc);
	//EndMg=Matrix();
	NextFlag=0;
	G_markpoint=Vector(0,0,0);
	if(ObjCnt)
	{
		for(Int32 i=0; i<ObjCnt; i++)
		{
			DeleteMem(mobj[i].vv);
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].Enum);
			DeleteMem(mobj[i].Snum);
		}
	}

	ObjCnt=0;
	DeleteMem(mobj);
	G_P1=Vector();
	Mp1=Vector();
	Drag=false;
	majer=0;

	CutSnapFlag=0;
    ExFlag=false;

	_Starget1=nullptr;
	_Starget2=nullptr;
	 SnapID1=-1;
	 SnapID2=-1;



}
//--------------------------------------------------------------------------------------------------------------------------
//tool終了時実行
void SnapAlign::FreeTool( BaseDocument* doc, BaseContainer& data)
{

	if(doc)
	{

        ResetPlaneAxis(doc,data);
		ResetTool();
		BaseDraw* bd = doc->GetActiveBaseDraw();
		GeData vd = bd->GetParameterData(BASEDRAW_DATA_PLANE_LEGACY_MODE);
		vd.SetInt32(false);
		bd->SetParameter(BASEDRAW_DATA_PLANE_LEGACY_MODE, vd, DESCFLAGS_SET_USERINTERACTION);
		InitFlag= false;
	}
	
	DescriptionToolData::FreeTool( doc, data);
}

///////////////////////////////////////////////////////////////////

Bool SnapAlign::GetDEnabling(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_ENABLE flags, const BaseContainer* itemdesc)
{
	if (!doc)     return false;

	
	switch (id[0].id)
	{
		case MDATA_SETAXIS:
			{
				if( NextFlag || data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc) || PlaneSet_Flag ) return false;
				else return true;
			}
		case MDATA_RESET:
			{
				if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc)|| NextFlag) return false;
				else return true;
			}
		case MDATA_DRAG_IDE_OBJ_TARGET:
			{
				if( NextFlag|| PlaneSet_Flag) return false;
				else return true;
			}
		case MDATA_BOOL_IDE_UVFIX:
			{
				if( data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_ALIGN && data.GetBool( MDATA_SNAP_AX) && data.GetBool( MDATA_SNAP_AY)&&data.GetBool( MDATA_SNAP_AZ) && !data.GetBool(MDATA_BOOL_OFFSET) && !data.GetBool(MDATA_BOOL_LINEALIGN) )
				{
					UV_Sw=true;
					return true;
				}
				else
				{
					UV_Sw=false;
					return false;
				}
			}
		case MDATA_SNAP_VALUEX:
			{
				if(data.GetBool(MDATA_BOOL_OFFSET)) return true;
				else return false;
			}
		case MDATA_SNAP_VALUEY:
			{
				if(data.GetBool(MDATA_BOOL_OFFSET)) return true;
				else return false;
			}
		case MDATA_SNAP_VALUEZ:
			{
				if(data.GetBool(MDATA_BOOL_OFFSET)) return true;
				else return false;
			}
		case MDATA_CHILD:
		{
			if(doc->GetMode() == Mobject || doc->GetMode() == Mmodel ) return true;
			else return false;
		}
		case MDATA_BOOL_COPYCNT:
			{
			if(data.GetInt32(MDATA_SNAP_MODE)==MDATA_CH_POSFIX && (doc->GetMode() ==Mpolygons || doc->GetMode() == Mmodel || doc->GetMode() == Mobject) ) return true;
			else if(data.GetInt32(MDATA_SNAP_MODE)==MDATA_CH_ALIGN && (doc->GetMode() ==Mpolygons || doc->GetMode() == Mmodel || doc->GetMode() == Mobject)) return true;
			else return false;
		}
		case MDATA_COPYINS:
			{
			if(doc->GetMode() == Mmodel || doc->GetMode() == Mobject ) return true;
			else return false;
		}
		case MDATA_RENDERINS:
		{
			if ((doc->GetMode() == Mmodel || doc->GetMode() == Mobject) && data.GetBool(MDATA_COPYINS)) return true;
			else return false;
		}
		case MDATA_BOOL_LINEALIGN:
			{
				if(data.GetInt32(MDATA_SNAP_MODE)==MDATA_CH_ALIGN) return true;
				else return false;
			}
		//case MDATA_BOOL_COPYPOLY:
		//	{
		//	if(doc->GetMode() ==Mpolygons || doc->GetMode() == Mmodel || doc->GetMode() == Mobject) return true;
		//	else return false;
		//}
		case IDE_Snap_AXIS:
		{
			if( NextFlag) return false;
			else return true;
		}
		//*********************************************************
	case MDATA_GRID_VALUE:
		{
			if(data.GetBool(MDATA_BOOL_GRID_FLAG))
			{
				return (MDATA_GRID_VALUE >= 0L);
			}
			else
			{
				return (MDATA_GRID_VALUE <= 0L);
			}
		}
	case MDATA_GRIDLINE :
		{
			if(data.GetBool(MDATA_BOOL_GRID_FLAG))
			{
				return (MDATA_GRIDLINE  >= 0L);
			}
			else
			{
				return (MDATA_GRIDLINE <= 0L);
			}
		}
	case MDATA_LINE :
		{
			if(data.GetBool(MDATA_BOOL_GRID_FLAG))
			{
				return (MDATA_LINE >= 0L);
			}
			else
			{
				return (MDATA_LINE  <= 0L);
			}
		}
	case MDATA_GRID_LIST :
		{
			if(data.GetBool(MDATA_BOOL_GRID_FLAG))
			{
				return (MDATA_GRID_LIST >= 0L);
			}
			else
			{
				return (MDATA_GRID_LIST  <= 0L);
			}
		}
		//////////////////////////////////////////////////
		//スナップ設定
	case SNAP_POINTG:
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_POINTG >= 0L);
			}
			else
			{
				return ( SNAP_POINTG<= 0L);
			}
		}
	case SNAP_SPLINEG :
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_SPLINEG  >= 0L);
			}
			else
			{
				return ( SNAP_SPLINEG <= 0L);
			}
		}
	case SNAP_MIDPOINTG:
		{
			if(data.GetBool(SNAP_ENABLEG)&& (data.GetBool(SNAP_SPLINEG) || data.GetBool(SNAP_EDGEG) || data.GetBool(SNAP_POLYGONG)))
			{
				return ( SNAP_MIDPOINTG >= 0L);
			}
			else
			{
				return ( SNAP_MIDPOINTG <= 0L);
			}
		}
	case SNAP_EDGEG:
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_EDGEG >= 0L);
			}
			else
			{
				return ( SNAP_EDGEG <= 0L);
			}
		}
	case SNAP_AXISG:
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_AXISG >= 0L);
			}
			else
			{
				return ( SNAP_AXISG <= 0L);
			}
		}
	case SNAP_POLYGONG:
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_POLYGONG >= 0L);
			}
			else
			{
				return ( SNAP_POLYGONG <= 0L);
			}
		}
	case SNAP_INTERSECTIONG :
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_INTERSECTIONG  >= 0L);
			}
			else
			{
				return ( SNAP_INTERSECTIONG  <= 0L);
			}
		}
	case SNAP_WORKPLANEG :
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return ( SNAP_WORKPLANEG  >= 0L);
			}
			else
			{
				return ( SNAP_WORKPLANEG  <= 0L);
			}
		}
	case SNAP_GRIDG :
		{
			if(data.GetBool(SNAP_ENABLEG) && data.GetBool(SNAP_WORKPLANEG))
			{
				return ( SNAP_GRIDG  >= 0L);
			}
			else
			{
				return ( SNAP_GRIDG  <= 0L);
			}
		}
	case SNAP_GRIDLINEG :
		{
			if(data.GetBool(SNAP_ENABLEG) && data.GetBool(SNAP_WORKPLANEG))
			{
				return ( SNAP_GRIDLINEG  >= 0L);
			}
			else
			{
				return ( SNAP_GRIDLINEG  <= 0L);
			}
		}
	case SNAP_GUIDE_SNAPG:
		{
			if(data.GetBool(SNAP_ENABLEG))
			{
				return (SNAP_GUIDE_SNAPG>= 0L);
			}
			else
			{
				return ( SNAP_GUIDE_SNAPG <= 0L);
			}
		}
	case SNAP_DYNAMICGUIDE_SNAPG:
		{
			if(data.GetBool(SNAP_GUIDE_SNAPG) && data.GetBool(SNAP_ENABLEG))
			{
				return (SNAP_DYNAMICGUIDE_SNAPG >= 0L);
			}
			else
			{
				return ( SNAP_DYNAMICGUIDE_SNAPG  <= 0L);
			}
		}
	case SNAP_PERPENDICULARG:
		{
			if(data.GetBool(SNAP_GUIDE_SNAPG)&& data.GetBool(SNAP_ENABLEG)&&data.GetBool(SNAP_DYNAMICGUIDE_SNAPG))
			{
				return (SNAP_PERPENDICULARG >= 0L);
			}
			else
			{
				return ( SNAP_PERPENDICULARG <= 0L);
			}
		}

	case MDATA_NORMALPOSITION:
		if( !PlaneSet_Flag && (doc->GetMode() == Mobject ||doc->GetMode()== Mmodel) ) return (MDATA_NORMALPOSITION >= 0L);
		else return (MDATA_NORMALPOSITION <= 0L);

	//case SNAP_PRE:
	//	if(doc->GetMode()== Mobject || doc->GetMode() == Mmodel) return (SNAP_PRE >= 0L);
	//	else return (SNAP_PRE <= 0L);

	case SNAP_SECTION:
		{
			if(data.GetBool(SNAP_ENABLEG)) return ( SNAP_SECTION >= 0L);
			else return ( SNAP_SECTION <= 0L);
		}
	case MDATA_ACTIONLIST:
		{
			if(data.GetInt32(MDATA_SNAP_MODE) ==  MDATA_CH_POSFIX && doc->GetMode() == Mpolygons) return true;
			else return false;
		}
		
	}
	
	//return DescriptionToolData::GetDEnabling(doc,data,id,t_data,flags,itemdesc);
	return SUPER::GetDEnabling(doc, data, id, t_data, flags, itemdesc);
}

//////////////////////////////////////////////////////////////////
//プラグイン状態
Int32 SnapAlign::GetState(BaseDocument *doc)
{

	return CMD_ENABLED;
}
//////////////////////////////////////////////////////////////
//アンドゥ設定
Bool SnapAlign::AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE type)
{
	if(!doc) return false;
	Int32 cnt=arr->GetCount();
	for(Int32 i=0; i<cnt; i++)
	{
		BaseObject *obj=(BaseObject*)arr->GetIndex(i);
		doc->AddUndo(type, obj);
	}
	return true;
}

//////////////////////////////////////////////////////////////////
//ToolBaseの実行が押された状態でパラメータを変えると実行される
Bool SnapAlign::DoCommand(ModelingCommandData &mdat)
{
	//return ModelingEdgeCut(mdat.arr,mdat.mode,mdat.bc,mdat.doc,NULL,NULL, (mdat.doc != NULL) && (mdat.flags & MODELINGCOMMANDFLAG_CREATEUNDO), NULL);
	return true;
}


//////////////////////////////////////////////////////////////////
Bool SnapAlign:: SetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags)
{
	if(flags == DESCFLAGS_SET_USERINTERACTION)
	{
		switch (id[0].id)
		{
			case MDATA_DRAG_IDE_OBJ_TARGET:
			{
				if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
				{
					ResetPlaneAxis(doc,data);
				}
			}
			break;
			case MDATA_SNAP_MODE:
			if (data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_POSFIX)
			{
				ResetTool();
			}
			break;
			case MDATA_SAVE:
			{
				Priinit(data, true);
			}
			break;
		}
	}

	return SUPER::SetDParameter(doc, data, id, t_data, flags);

}


////////////////////////////////////////////////////////////////
//キーボード設定
 Bool SnapAlign::KeyboardInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg)
 {
	Int32 key = msg.GetData(BFM_INPUT_CHANNEL).GetInt32();

	if(key == KEY_ESC)
	{
	
		ResetTool();
	}


	EventAdd();
	return false;
 }
////////////////////////////////////////////////////////////////
//描画
TOOLDRAW SnapAlign::Draw(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, BaseThread* bt, TOOLDRAWFLAGS flags)
{
	
	if (!(flags & TOOLDRAWFLAGS_HIGHLIGHT)) 
	return TOOLDRAW_0;


	DTOOL DP;


	bd->SetMatrix_Matrix(NULL, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->SetTransparency(0);
	bd->SetPen(Vector(1.0,0,0));

	//CTRLキーでAXIS軸表示
	if(CtrlOn)
	{
	   //	if(!DispMarkerB( doc,bd, bh, data)) return TOOLDRAW_0;
		if (!DispAXIS(doc, bd, bh, data)) return TOOLDRAW_0;

		bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);

		bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));		
		bd->SetPen(Vector(255,225,0));
		Vector PSp=bd->WS(MMP);
		//			DTOOL dp;
		//dp.printcheck("Psp=",PSp);
		Float ra=data.GetInt32(SNAP_RADIUSG);
		bd->DrawCircle2D((Int32)PSp.x, (Int32)PSp.y, ra);
		bd->SetPen(Vector(1.0,0,0));

		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		bd->DrawHandle(MMP, DRAWHANDLE_MIDDLE, 0);
		if(data.GetBool(MDATA_BOOL_OFFSET))
		{	
			
			
			if(data.GetBool(MDATA_SNAP_AX))
			{
				String sx=String::FloatToString(data.GetFloat(MDATA_SNAP_VALUEX));
				sx= sx+" ";
				DP.DrawTextA(sx ,lineV[0] ,HUDOffset , bd, EdgeID );
				//GePrint(sx);
			}
			if(data.GetBool(MDATA_SNAP_AY))
			{
				String sy=String::FloatToString(data.GetFloat(MDATA_SNAP_VALUEY));
				sy= sy+" ";
				DP.DrawTextA(sy ,lineV[1] ,HUDOffset, bd, EdgeID );
			}
			if(data.GetBool(MDATA_SNAP_AZ))
			{
				String sz=String::FloatToString(data.GetFloat(MDATA_SNAP_VALUEZ));
				sz= sz+" ";
				DP.DrawTextA(sz ,lineV[2] ,HUDOffset, bd, EdgeID );
			}
		}

		if(MenuDrag != -1)
		{
		
			
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->SetPen(Vector(1.0,0.0,1.0));
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.5));

			Vector MenuV2=bd->WS(G_markpoint);
			//MenuV2.z=0;
			bd->DrawLine2D(MenuV1,MenuV2);

		
			if(MenuDrag==1)
			{
				DP.DrawTextA("Snap Setting" ,G_markpoint ,HUDOffset , bd, 1 );
			}
			if(MenuDrag==2)
			{
				DP.DrawTextA("Align Mode" ,G_markpoint ,HUDOffset, bd, 1 );
			}
			if(MenuDrag==3)
			{
				DP.DrawTextA(  "Move Mode" ,G_markpoint ,HUDOffset , bd, 1 );
			}
			if(MenuDrag==4)
			{
				DP.DrawTextA( "Option Setting" ,G_markpoint ,HUDOffset , bd, 1 );
			}	
		}
	}
	else
	{
		DP.DispMarker(doc, bd, bh, data, EndMg, G_markpoint, PlaneSet_Flag, G_SnapFlag, PlaneSet_BTNFlag, FaceMg, _MX, _MY, 0);
		//Plane設定ボタンを押した場合
		if(PlaneSet_BTNFlag)
		{
			
			if(PlaneSetCnt==1)
			{
				
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->SetPen(Vector(0.0,1.0,1.0));
				bd->DrawHandle(PlV[0],DRAWHANDLE_BIG, 0);
				bd->SetPen(Vector(0.0,0.0,1.0));
				if(G_SnapFlag) bd->DrawLine(PlV[0],G_markpoint, 0);
			}
			else if(PlaneSetCnt==2)
			{
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->SetPen(Vector(0.0,1.0,1.0));
				bd->DrawHandle(PlV[0],DRAWHANDLE_BIG, 0);
				bd->DrawHandle(PlV[1],DRAWHANDLE_BIG, 0);
				bd->SetPen(Vector(0.0,0.0,1.0));

				bd->DrawLine(PlV[0],PlV[1], 0);
				if(G_SnapFlag) bd->DrawLine(PlV[0],G_markpoint, 0);
			}
		}
		else
		{
			//Alignモード
			if(data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_ALIGN )
			{
				bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
				if(data.GetBool(MDATA_BOOL_LINEALIGN))
				{
					if(NextFlag == 1)
					{
						if (doc->GetMode() != Medges)
						{
							if (!AddDrawViewPoint(doc, data, bd, bh)) return TOOLDRAW_0;
						}
						else
						{
							if (!AddDrawEdgeExtension(doc, data, bd, bh)) return TOOLDRAW_0;
						}
					}
				}
				else
				{
					if(!AddDrawViewPoint(doc, data, bd, bh )) return TOOLDRAW_0;
				}
			}
			//Moveモード
			else if(data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_POSFIX)
			{
				Int32 cutCnt=data.GetInt32(SNAP_SECTION );

				Float sx= data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
				Float sy= data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
				Float sz= data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);
				if(NextFlag==1 )
				{
				
					Vector tgv;
					bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
					if(Drag)
					{
						bd->SetMatrix_Matrix(NULL, bh->GetMg());
						bd->DrawLine(Mp1,G_markpoint,0 );

                        SPANV[0]=G_markpoint;
				        SPANV[1]=Mp1;
				        Vector unit=(G_markpoint-Mp1)/cutCnt;
				        Vector fv=Mp1;
				        for(Int32 i=1; i< cutCnt; i++)
				        {
					        bd->DrawHandle(fv+unit*i, DRAWHANDLE_BIG, 0);
				        }

				        bd->DrawHandle(Mp1, DRAWHANDLE_BIG, 0);
				        if(cutCnt==2)
                        {
                             tgv=(G_markpoint+Mp1)/2;
                        }
                        else
                        {
					        CutSnapFlag=1;
                            tgv=G_markpoint;
				        }

					}
					else
					{
						if(AltKey)
						{
							tgv=LocV;
						}
						else
						{
							tgv=G_markpoint;
							if (CutSnapFlag == 2) tgv = MMP;
						}
					}

					if(G_SnapFlag)
					{
						Vector pos=tgv-G_P1;
						Matrix Smg=EndMg;
						Smg.off=Vector();
						pos=~Smg*pos;
						Vector vc=Vector(0,0,0);
						if(data.GetBool(MDATA_SNAP_AX))
						{
							if(data.GetBool(MDATA_BOOL_OFFSET))
							{
								pos.x=pos.x+sx;
							}
							vc=vc+Vector(pos.x,0,0);
						}
						if(data.GetBool(MDATA_SNAP_AY))
						{
							if(data.GetBool(MDATA_BOOL_OFFSET))
							{
								pos.y=pos.y+sy;
							}
							vc=vc+Vector(0,pos.y,0);
						}
						if(data.GetBool(MDATA_SNAP_AZ))
						{
							if(data.GetBool(MDATA_BOOL_OFFSET))
							{
								pos.z=pos.z+sz;
							}
							vc=vc+Vector(0,0,pos.z);
						}
						vc=Smg*vc;
						Vector get=vc+G_P1;

							//-------------------------------------
						bd->SetMatrix_Matrix(NULL, bh->GetMg());
						bd->DrawLine(G_P1,get, 0);
						bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
						SetVectorLen(doc, data, G_P1, get);
                        

                        Vector LVP1=~EndMg*G_P1;
						Vector LVP2 = ~EndMg*get;
                        Float Le=Len(LVP2-LVP1);
                        String s="Dis = "+String::FloatToString(Le);
                    

                        DP.DrawTextA(s,get, HUDOffset, bd, 1);

						/////////////////////////////////////
						//分割スナップが２若しくは
						if(cutCnt == 2|| CutSnapFlag==2)
						{
							bd->SetMatrix_Matrix(NULL, bh->GetMg());
							bd->DrawLine(G_P1,get, 0);
							bd->DrawHandle(get, DRAWHANDLE_BIG, 0);
						}
						////////////////////////////////////
						


						if(data.GetBool( SNAP_PRE))
						{
							Vector colar=data.GetVector(PRE_COLOR);
							AutoAlloc<AtomArray> arr;
							if (!arr) return TOOLDRAW_0;
							doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
							Int32 cnt=arr->GetCount();
							Vector rl=get-G_P1;
							for(Int32 i=0; i<cnt; i++)
							{
								BaseObject *op=(BaseObject *)arr->GetIndex(i);
								bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_LOWEREQUAL);	
								ModelingCommandData mcd;
								mcd.doc = doc;
								mcd.op = op;
								if(!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, mcd)) return TOOLDRAW_0;
								BaseObject*cp = static_cast<BaseObject*>(mcd.result->GetIndex(0));
								
								BaseObject*res=NULL;
								if(cp->GetDown())
								{
									AutoAlloc <AtomArray> arr2;
									arr2->Append(cp);
									//DTOOL DP;
									DP.search_hierarchy( cp->GetDown(), cp, arr2);

									BaseContainer bc;
									bc.SetBool(MDATA_JOIN_MERGE_SELTAGS, true);

									ModelingCommandData mcd2;
									mcd2.doc = NULL;
									mcd2.arr=arr2;
									mcd2.bc=&bc;
									mcd2.mode = MODELINGCOMMANDMODE_ALL;
									if(!SendModelingCommand(MCOMMAND_JOIN, mcd2)) return TOOLDRAW_0;
									res = static_cast<BaseObject*>(mcd2.result->GetIndex(0));
									AtomArray::Free(mcd2.result);
									Matrix rx;
									BaseObject::Free(cp);
									cp=NULL;
									cp=res;
									cp->SetMg(rx);
								}
								Matrix zmg;
								if(cp->GetType() == Opolygon)
								{
									if(doc->GetMode() == Mpolygons)
									{
										PolygonObject* poly= ToPoly(cp);
										BaseSelect* bs = poly->GetPolygonS();
										if(bs->GetCount())
										{
											AutoAlloc<Modeling> mod;
											if (!mod || !mod->InitObject(cp)) return TOOLDRAW_0;
											for(Int32 j=0; j< poly->GetPolygonCount(); j++)
											{
												if(!bs->IsSelected(j))
												{
													mod->DeleteNgon(cp, j, true);
												}
											}
											if (!mod->Commit()) return  TOOLDRAW_0;
										}
									}
									Matrix am=op->GetMg();
									//DTOOL DP;
									Vector scl=DP.GetScaleForMatrix(am);
									zmg=MatrixScale(scl);
									if(data.GetBool( MDATA_NORMALPOSITION))
									{
										zmg=Smg*zmg;
									}
									else
									{
										zmg=Smg*am;
									}
									zmg.off=am.off+rl;
									//Matrix am=cp->GetMg();
									//am=Smg*am;
									//am.off=am.off+rl;
									cp->SetMg(zmg);
									if(bd->DrawPolygonObject(bh,cp, DRAWOBJECT_XRAY_ON | DRAWOBJECT_USE_CUSTOM_COLOR | DRAWOBJECT_NO_EOGL, NULL, colar) == DRAWRESULT_ERROR) return TOOLDRAW_0;
									
								}
								
								if(PShift)
								{
									if(doc->GetMode() == Mmodel || doc->GetMode() == Mobject)
									{
										//Vector pos=zmg.off;
										for(Int32 j=1; j<data.GetInt32(MDATA_BOOL_COPYCNT); j++)
										{
											AutoAlloc<AliasTrans> trans;
											BaseObject* Clone=(BaseObject*)cp->GetClone(COPYFLAGS_0,trans);
											if(Clone->GetType() == Opolygon)
											{
												zmg.off=pos+(rl*j);
												Clone->SetMg(zmg);
												if(bd->DrawPolygonObject(bh,Clone, DRAWOBJECT_XRAY_ON | DRAWOBJECT_USE_CUSTOM_COLOR | DRAWOBJECT_NO_EOGL, NULL, colar) == DRAWRESULT_ERROR) return TOOLDRAW_0;
											}
											BaseObject::Free(Clone);
											Clone=NULL;
										}
									}
								}
								BaseObject::Free(cp);
								cp=NULL;
							}
						}
					}
					else
					{
						bd->SetMatrix_Matrix(NULL, bh->GetMg());
						if (!CutSnapFlag)
						{
							bd->DrawLine(G_P1, G_markpoint, 0);
							bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
						}
						
						SetVectorLen(doc, data, G_P1, G_markpoint);
					
					}

				}

				//分割スナップマーカー表示
				if (CutSnapFlag)
				{

					bd->SetMatrix_Matrix(NULL, bh->GetMg());
					bd->SetPen(Vector(255, 255, 255));
					bd->DrawHandle(SPANV[1], DRAWHANDLE_BIG, 0);
					bd->DrawHandle(SPANV[0], DRAWHANDLE_BIG, 0);
					bd->DrawLine(SPANV[1], SPANV[0], 0);

					cutCnt = data.GetInt32(SNAP_SECTION);	//分割スナップのカウント取得
					Vector unit = (SPANV[0] - SPANV[1]) / cutCnt;
					Vector fv = SPANV[1];

					//DTOOL DP;
					for (Int32 i = 1; i< cutCnt; i++)
					{
						Vector pv = fv + unit*i;
						if (DP.FuzzyPointInRange(data, bd, pv, MpRX[0], MpRX[1]))
						{
							bd->SetPen(Vector(255, 0, 0));
						}
						else
						{
							bd->SetPen(Vector(0, 0, 255));
						}
						bd->DrawHandle(fv + unit*i, DRAWHANDLE_BIG, 0);
					}
				}

				if (Drag)
				{
					bd->SetMatrix_Matrix(NULL, bh->GetMg());
					bd->SetPen(Vector(255, 255, 255));
					bd->DrawHandle(Mp1, DRAWHANDLE_BIG, 0);
					bd->DrawHandle(G_markpoint, DRAWHANDLE_BIG, 0);
					bd->DrawLine(Mp1, G_markpoint, 0);

					cutCnt = data.GetInt32(SNAP_SECTION);	//分割スナップのカウント取得
					Vector unit = (Mp1 - G_markpoint) / cutCnt;
					Vector fv = G_markpoint;

					//DTOOL DP;
					for (Int32 i = 1; i< cutCnt; i++)
					{
						//Vector pv = fv + unit*i;
	
						bd->SetPen(Vector(0, 0, 255));
						bd->DrawHandle(fv + unit*i, DRAWHANDLE_BIG, 0);
					}

				}
			}
		}


		//メジャーモード有効の場合
		if(majer)
		{
			//	GePrint("mager");
			Float sx= data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
			Float sy= data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
			Float sz= data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);
			Vector tgv;
			bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
			if(Drag)
			{
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->DrawLine(Mp1,G_markpoint, 0);
				bd->DrawHandle((G_markpoint+Mp1)/2, DRAWHANDLE_BIG, 0);
				bd->DrawHandle(Mp1, DRAWHANDLE_BIG, 0);
				tgv=(G_markpoint+Mp1)/2;
			}
			else
			{
				if(AltKey)
				{
					tgv=LocV;
				}
				else
				{
					tgv=G_markpoint;
					if (CutSnapFlag == 2) tgv = MMP;
				}
			}
            
            
			if(G_SnapFlag)
			{
				
				Vector pos=tgv-G_P1;
				Matrix Smg=EndMg;
				Smg.off=Vector();
				pos=~Smg*pos;
				Vector vc=Vector(0,0,0);
				if(data.GetBool(MDATA_SNAP_AX))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.x=pos.x+sx;
					}
					vc=vc+Vector(pos.x,0,0);
				}
				if(data.GetBool(MDATA_SNAP_AY))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.y=pos.y+sy;
					}
					vc=vc+Vector(0,pos.y,0);
				}
				if(data.GetBool(MDATA_SNAP_AZ))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.z=pos.z+sz;
					}
					vc=vc+Vector(0,0,pos.z);
				}
				vc=Smg*vc;
				Vector get=vc+G_P1;

				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->DrawLine(G_P1,get, 0);
				//bd->DrawHandle(get, DRAWHANDLE_BIG);
				bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
				SetVectorLen(doc, data, G_P1, get);

	 			Get2=get;
           
                Vector LVP1=~EndMg*G_P1;
                Vector LVP2=~EndMg*get;
                Float Le=Len(LVP2-LVP1);
                String s="Dis = "+String::FloatToString(Le);
        
                DP.DrawTextA(s,get, HUDOffset, bd, 1);
			}
			else
			{
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->DrawLine(G_P1,G_markpoint, 0);
				bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
				SetVectorLen(doc, data, G_P1, G_markpoint);
			}


		}

	
		
	}
	
	return TOOLDRAW_HANDLES|TOOLDRAW_AXIS;
}



//****************************************************************************************
//ガイドAXIS表示
//****************************************************************************************
Bool SnapAlign::DispAXIS(BaseDocument *doc, BaseDraw *bd, BaseDrawHelp *bh, BaseContainer &data)
{
	if(!doc) return false;
	bd->SetMatrix_Matrix(NULL, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(2.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->SetTransparency(0);
	//ローカルマトリックスに現在の位置を代入しスケールを求める
	Matrix point_mg;
	point_mg = EndMg;

	//GePrint("point_mg.x="+FloatToString(point_mg.off.x)+"point_mg.y="+FloatToString(point_mg.off.y)+"point_mg.z="+FloatToString(point_mg.off.z));
	Int32 left, top, right, bottom, width, height;
	Vector ctr = bd->WS(MemoPos);
	
	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Float rad = (Float)((width > height)?(height>>2):(width>>2));
	
      // Scale for PolygonObject
	Int32 proj =	bd->GetProjection();
	Vector a = ScreenToWorld(ctr, bd, proj);
	Vector b = ScreenToWorld(Vector(ctr.x+rad*0.1, ctr.y, ctr.z), bd, proj);
	Float rad2 = 2.0 * Len(b-a);
	if (proj == Pfrog)
    {
		point_mg.v1 = !point_mg.v1 * rad2;
		point_mg.v2 = !point_mg.v2 * rad2 * 0.333;
		point_mg.v3 = !point_mg.v3 * rad2;
	}
	else
	{
		point_mg.v1 = !point_mg.v1 * rad2;
		point_mg.v2 = !point_mg.v2 * rad2;
		point_mg.v3 = !point_mg.v3 * rad2;
	}
	//軸ラインの長さ
	//Vector Aline[3];
	Float len;

	//カラー設定
	Vector Color[3];
	Color[0] = Vector(1,0,0);
	Color[1] = Vector(0,1,0);
	Color[2] = Vector(0,0,1);

	//Axis軸モード有効時Axis描画設定
	if(CtrlOn)
	{
//		DTOOL DP;
//		Vector r = MatrixToHPB(point_mg, ROTATIONORDER_DEFAULT);
//		DP.printcheckR("point_mgt Rot=", r);
		BaseDraw *bbd = doc->GetActiveBaseDraw();
		Int32 view =	bbd->GetProjection();
		if(view ==0)
		{
			len=50;
			Vector rot=MatrixToHPB(EndMg, ROTATIONORDER_DEFAULT);
			point_mg=HPBToMatrix(rot,ROTATIONORDER_DEFAULT);		
		}
		else
		{
			len=2.5;
		}
		point_mg.off = AxisPos;

		Aline[0]=Vector(len,0,0);
		Aline[1]=Vector(0,len,0);
		Aline[2]=Vector(0,0,len);


		bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(3.0));
		//XYZのチェック
		if(!data.GetBool(MDATA_SNAP_AX))
		{
			Color[0] = Vector(0.3,0,0);
		}
		if(!data.GetBool(MDATA_SNAP_AY))
		{
			Color[1] = Vector(0,0.3,0);
		}
		if(!data.GetBool(MDATA_SNAP_AZ))
		{
			Color[2] = Vector(0,0,0.3);
		}
        
        //GePrint("EdgeID="+String::IntToString(EdgeID));
	
		if(EdgeID== 1 || EdgeID== 4)
		{
			Color[0] = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
		}
		else if(EdgeID== 3 || EdgeID== 8)
		{
			Color[1] = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
		}
		else if(EdgeID== 7 || EdgeID== 9)
		{
			Color[2] = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
		}
        
         else if(EdgeID== 150)
         {
             Color[0]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
             Color[1]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
         }
        else if(EdgeID== 160)
        {
            Color[1]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
            Color[2]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
        }
        else if(EdgeID== 170)
        {
            Color[0]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
            Color[2]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
        }

        Vector ZV[3];
		for(Int32 i=0; i<3; i++)
		{
            ZV[i]=Aline[i];
        }


		for(Int32 i=0; i<3; i++)
		{
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
            switch(i)
            {
                case 0:
                    $VectorXYZ[i]=Vector(ZV[0].x,ZV[1].y,0);
                    break;
                case 1:
                    $VectorXYZ[i]=Vector(0,ZV[1].y,ZV[2].z);
                    break;
                case 2:
                    $VectorXYZ[i]=Vector(ZV[0].x,0,ZV[2].z);
                    break;
            }
            $VectorXYZ[i]=point_mg*$VectorXYZ[i];
			Aline[i]=point_mg*Aline[i];
			lineV[i]=Aline[i];
			if(EdgeID==30)
			{
				Color[i]=GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
			}
			bd->SetPen(Color[i]);
			bd->DrawLine(point_mg.off,Aline[i],0);

		}
        
        
        bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
  
 
		for(Int32 i=0; i<3; i++)
		{
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
            Vector ColorV;
            if(i==0 &&EdgeID== 150)
            {
                ColorV  = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
            }
            else if(i==1 &&EdgeID== 160)
            {
                ColorV  = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
            }
            else if(i==2 &&EdgeID== 170)
            {
                ColorV  = GetViewColor(VIEWCOLOR_SELECTION_PREVIEW);
            }
            else
            {
                if(i==0)
                {
                    if(data.GetBool(MDATA_SNAP_AX ) && data.GetBool(MDATA_SNAP_AY ) )
                    {
                        ColorV = Vector(1,0,0);
                    }
                    else
                    {
                        ColorV = Vector(0.3,0,0);
                    }
                }
                else if(i==1)
                {
                    
                    if(data.GetBool(MDATA_SNAP_AY ) && data.GetBool(MDATA_SNAP_AZ ))
                    {
                        ColorV = Vector(0,1,0);
                    }
                    else
                    {
                        ColorV = Vector(0,0.3,0);
                    }
                }
                else
                {
                    if(data.GetBool(MDATA_SNAP_AZ ) && data.GetBool(MDATA_SNAP_AX ) )
                    {
                        ColorV = Vector(0,0,1);
                    }
                    else
                    {
                        ColorV = Vector(0,0, 0.3);
                    }
                }
            }
            bd->SetPen(ColorV );
            
            switch(i)
            {
                case 0:
                    bd->DrawLine(Aline[0],$VectorXYZ[0],0);
                    bd->DrawLine(Aline[1],$VectorXYZ[0],0);
                    break;
                case 1:
                    bd->DrawLine(Aline[1],$VectorXYZ[1],0);
                    bd->DrawLine(Aline[2],$VectorXYZ[1],0);
                    break;
                case 2:
                    bd->DrawLine(Aline[0],$VectorXYZ[2],0);
                    bd->DrawLine(Aline[2],$VectorXYZ[2],0);
                    break;
            }

				
			if(EdgeID==100 && i==0 )
			{
				bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
			}
			else if(EdgeID==200  && i==1 )
			{
			   bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
			}
			else if(EdgeID==300  && i==2 )
			{
			   bd->SetPen(GetViewColor(VIEWCOLOR_SELECTION_PREVIEW));
			}
			else 
			{
				 bd->SetPen(Vector(1, 0.5, 0.3) );
			}
			Vector h = bd->WS( Aline[i]); 
			 bd->DrawHandle2D( h,DRAWHANDLE_BIG );
        }

		bd->SetPen(Vector(1,1,1));
		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		bd->DrawHandle(point_mg.off,DRAWHANDLE_BIG, 0);

	}
	else
	{
		//len=1.5;
		//Aline[0]=Vector(len,0,0);
		//Aline[1]=Vector(0,len,0);
		//Aline[2]=Vector(0,0,len);
		////軸ライン表示
		//for(Int32 i=0; i<3; i++)
		//{
		//	Aline[i]=point_mg*Aline[i];
		//	bd->SetPen(Color[i]);
		//	bd->DrawLine(point_mg.off,Aline[i], 0);
		//}
		Matrix Apoint_mg =EndMg;
		//if (PlaneSet_Flag) Apoint_mg = SNmg;

		Vector Actr;
		Actr = bd->WS(Apoint_mg.off);
		Vector a2, b2;
		a2 = ScreenToWorld(Actr, bd, proj);
		b2 = ScreenToWorld(Vector(Actr.x + rad*0.1, Actr.y, Actr.z), bd, proj);
		Float Arad2;
		Arad2 = 2.0 * Len(b2 - a2);
		if (proj == Pfrog)
		{
			Apoint_mg.v1 = !Apoint_mg.v1 * Arad2;
			Apoint_mg.v2 = !Apoint_mg.v2 * Arad2 * 0.333;
			Apoint_mg.v3 = !Apoint_mg.v3 * Arad2;
		}
		else
		{
			Apoint_mg.v1 = !Apoint_mg.v1 * Arad2;
			Apoint_mg.v2 = !Apoint_mg.v2 * Arad2;
			Apoint_mg.v3 = !Apoint_mg.v3 * Arad2;
		}
		bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(2.0));
		//軸ラインの長さ
		Vector Aline[3];
		Float len = 1.5;
		Aline[0] = Vector(len, 0, 0);
		Aline[1] = Vector(0, len, 0);
		Aline[2] = Vector(0, 0, len);
		Vector Lpos = EndMg.off;
		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		//軸ライン表示
		for (Int32 i = 0; i<3; i++)
		{

			Aline[i] = Apoint_mg*Aline[i];
			bd->SetPen(Color[i]);
			bd->DrawLine(Lpos, Aline[i], NOCLIP_D);

		}
	}
	return true;

}

////////////////////////////////////////////////////////
//スクリーンからワールド変換
Vector SnapAlign::ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection)
//*---------------------------------------------------------------------------*
{
     Vector p = pp;

     Vector     off, scale, scalez;
     bd->GetViewParameter(&off, &scale, &scalez);
     switch (projection)
     {
          case Pmilitary: case Pfrog: case Pbird: case Pgentleman:
          p.x -= p.z*scale.x*scalez.x;
          p.y += p.z*scale.y*scalez.y;
          break;
     }

     p.x = (p.x-off.x)/scale.x;
     p.y = (p.y-off.y)/scale.y;

     if (projection==Pperspective)
     {
          Float nz = p.z + 0.05;
          p.x *= nz;
          p.y *= nz;
     }
     return bd->GetMg()*p;
}
//-------------------------------------------------------------------------------------
//メッセージ
Bool SnapAlign::Message(BaseDocument* doc, BaseContainer& data, Int32 type, void* t_data)
{

	switch(type)
	{
		case  MSG_GETCUSTOMICON:
		{
			if(InitFlag)
			{
			//メインスナップ設定をツールスナップ設定に反映
				DTOOL DP;
				if (!DP.CheckSnapSetting(doc, data)) return false;
			}
		}
		break;
		case MSG_DESCRIPTION_USERINTERACTION_END:
			{

			}
			break;
		//ドロップダウンメニュー実行時に実行
		case MSG_DESCRIPTION_POSTSETPARAMETER:
			{
				if (NextFlag == 0)
				{
				
					//ツールの平面タイプを変更した場合に即座に反映
					BaseDraw* bd = doc->GetActiveBaseDraw();
					DTOOL DP;
					EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg, 0);//　基準平面を設定し最終マトリックスを返す
				}
				DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
				EventAdd();
			}
		break;

		case MSG_DESCRIPTION_COMMAND:
		{
			DescriptionCommand *dc = (DescriptionCommand*) t_data;
			switch(dc->id[0].id)
			{
			case MDATA_SETAXIS:
				FaceMg = EndMg;
					Set3pointPlane(doc,data);
				break;
			case MDATA_RESET:
				ResetPlaneAxis(doc, data);
				break;
			case MDATA_BTN_VRESET:
				VReset(data);
				break;

			case MDATA_SAVE:
				TEST(doc->GetActiveObject());
				//	Priinit( data, true );
				break;
			}
		}
		break;
		case MSG_DESCRIPTION_CHECKUPDATE:
		{
			//ツールスナップ設定をシステムスナップ設定に反映させる
	//**********************************************************************************************
			DTOOL DP;
			DP.SetCHECKUP(doc, data,PlaneSet_BTNFlag);
			EnableSnap( data.GetBool(SNAP_POINTG), doc, SNAPMODE_POINT);
			EnableSnap( data.GetBool( SNAP_SPLINEG ), doc, SNAPMODE_SPLINE );
			EnableSnap( data.GetBool( SNAP_MIDPOINTG ), doc, SNAPMODE_MIDPOINT);
			EnableSnap( data.GetBool( SNAP_EDGEG ), doc, SNAPMODE_EDGE);
			EnableSnap( data.GetBool( SNAP_AXISG ), doc, SNAPMODE_AXIS );
			EnableSnap( data.GetBool( SNAP_POLYGONG ), doc, SNAPMODE_POLYGON );
			EnableSnap( data.GetBool( SNAP_INTERSECTIONG  ), doc, SNAPMODE_INTERSECTION );
			EnableSnap( data.GetBool( SNAP_WORKPLANEG  ), doc, SNAPMODE_WORKPLANE );
			EnableSnap( data.GetBool( SNAP_GRIDG ), doc, SNAPMODE_GRIDPOINT );
			EnableSnap( data.GetBool( SNAP_GRIDLINEG), doc, SNAPMODE_GRIDLINE );
			EnableSnap( data.GetBool( SNAP_GUIDE_SNAPG ), doc, SNAPMODE_GUIDE );
			EnableSnap( data.GetBool( SNAP_DYNAMICGUIDE_SNAPG ), doc, SNAPMODE_DYNAMICGUIDE );
			EnableSnap( data.GetBool( SNAP_PERPENDICULARG ), doc, SNAPMODE_ORTHO );

	//**********************************************************************************************	
		}
		break;

	}

	return true;
}

Bool SnapAlign::GetDDescription(BaseDocument* doc, BaseContainer& data, Description* description, DESCFLAGS_DESC& flags)
{
	if (! description->LoadDescription("SnapTransfer") ) return false;

	//スナップ設定を初期化
	//DTOOL DP;
	//if(!DP.CheckSnapSetting( doc,data)) return false;

	BaseContainer *bc2 = description->GetParameterI(DescLevel(IDE_Snap_AXIS), NULL);
	if (bc2) bc2->SetBool(DESC_HIDE, true);


	DescID did3 = DescLevel(MDATA_SAVE, DTYPE_NONE, 0);
	BaseContainer settings3;
	settings3.SetInt32(DESC_CUSTOMGUI, CUSTOMGUI_BITMAPBUTTON);
	settings3.SetString(DESC_NAME, "Set Default:");
	settings3.SetBool(BITMAPBUTTON_BUTTON, true);
	settings3.SetInt32(DESC_GUIOPEN, true);
	settings3.SetBool(BITMAPBUTTON_BUTTON, true);
	settings3.SetBool(BITMAPBUTTON_TOGGLE, true);
	settings3.SetString(BITMAPBUTTON_TOOLTIP, "Set As Default");
	settings3.SetInt32(BITMAPBUTTON_ICONID1, ID_BITMAPBTN_SAVE);  //Uses a custom icon we registered in the register() method

	if (!description->SetParameter(did3, settings3, DescLevel(MDATA_SAVE))) return false;


	flags |= DESCFLAGS_DESC_LOADED;
	return ToolData::GetDDescription(doc, data, description, flags);
}


Bool SnapAlign::GetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags)
{
	switch (id[0].id)
	{

	case MDATA_SAVE:

		flags |= DESCFLAGS_GET_PARAM_GET;
		break;
	}
	return SUPER::GetDParameter(doc, data, id, t_data, flags);
}

///////////////////////////////////////////////////////////////////////////////////
//クリックする度に実行
Bool SnapAlign::MouseInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg)
{

	if (!doc) return false;
	Drag=false;
	//最初のクリック位置（スクリーン座標）の取得	
	Float mx = msg.GetFloat(BFM_INPUT_X);
	Float my = msg.GetFloat(BFM_INPUT_Y);

	Int32 button = NULL;

	//クリックボタンの取得
	switch (msg.GetInt32(BFM_INPUT_CHANNEL))
	{
		case BFM_INPUT_MOUSELEFT : button=KEY_MLEFT; break;
		case BFM_INPUT_MOUSERIGHT: button=KEY_MRIGHT; break;
		case BFM_INPUT_MOUSEMIDDLE: button=KEY_MMIDDLE; break;
		case BFM_INPUT_MOUSEX1: button=KEY_MX1; break;
		default: break;
	}


	//左ボタン以外は無効
	if(button != KEY_MLEFT)	 return false;
	
	SnapResult	snapResul = SnapResult();
	Matrix      guideMatrix = Matrix();
	DTOOL DP;

	//軸表示モード
	if(CtrlOn)
	{
		if(EdgeID== 1 || EdgeID== 4)
		{
			if(data.GetBool(MDATA_SNAP_AX))
			{
				data.SetBool(MDATA_SNAP_AX, false);
			}
			else
			{
				data.SetBool(MDATA_SNAP_AX, true);
			}
		}
		else if(EdgeID== 3 || EdgeID== 8)
		{
			if(data.GetBool(MDATA_SNAP_AY))
			{
				data.SetBool(MDATA_SNAP_AY, false);
			}
			else
			{
				data.SetBool(MDATA_SNAP_AY, true);
			}
		}
		else if(EdgeID== 7 || EdgeID== 9)
		{
			if(data.GetBool(MDATA_SNAP_AZ))
			{
				data.SetBool(MDATA_SNAP_AZ, false);
			}
			else
			{
				data.SetBool(MDATA_SNAP_AZ, true);
			}
		}
		else if(EdgeID==30)
		{
			if(data.GetBool(MDATA_BOOL_OFFSET))
			{
				data.SetBool(MDATA_BOOL_OFFSET, false);
			}
			else
			{
				data.SetBool(MDATA_BOOL_OFFSET, true);
			}		
		}
		else if(EdgeID==100)
		{
			if(Odlg_flag == 2 || Odlg_flag == 3)
			{
				Odlg.Close(false);
			}
			Odlg.SetAdd(1);
			Odlg.Open(-1,-1,0,0);
			Odlg_flag=1;
		}
		else if(EdgeID==200)
		{
			if(Odlg_flag == 1 || Odlg_flag == 3)
			{
				Odlg.Close(false);
			}
			Odlg.SetAdd(2);
			Odlg.Open(-1,-1,0,0);
			Odlg_flag=2;
		}
		else if(EdgeID==300)
		{
			if(Odlg_flag == 1 || Odlg_flag == 2)
			{
				Odlg.Close(false);
			}
			Odlg.SetAdd(3);
			Odlg.Open(-1,-1,0,0);
			Odlg_flag=3;
		}
        else if(EdgeID==150)
        {
            static Bool _Xflag=false;
            if(_Xflag)
            {
                _Xflag=false;
            }
            else
            {
                _Xflag = true;
            }
            data.SetBool(MDATA_SNAP_AX, _Xflag);
            data.SetBool(MDATA_SNAP_AY, _Xflag);
            data.SetBool(MDATA_SNAP_AZ, !_Xflag);

        }
        else if(EdgeID==160)
        {
            static Bool _Yflag=false;
            if(_Yflag)
            {
                _Yflag=false;
            }
            else
            {
                _Yflag = true;
            }
            data.SetBool(MDATA_SNAP_AZ, _Yflag);
            data.SetBool(MDATA_SNAP_AY, _Yflag);
            data.SetBool(MDATA_SNAP_AX, !_Yflag);
        }
        else if(EdgeID==170)
        {
            static Bool _Zflag=false;
            if(_Zflag)
            {
                _Zflag=false;
            }
            else
            {
                _Zflag = true;
            }
            data.SetBool(MDATA_SNAP_AZ, _Zflag);
            data.SetBool(MDATA_SNAP_AX, _Zflag);
            data.SetBool(MDATA_SNAP_AY, !_Zflag);
        }

		
		Float dx,dy;

		BaseContainer bc;
		BaseContainer device;
		MenuV1=Vector(mx,my,0);
		MenuDrag=0;
		Vector DragPos;
		
		////マウスドラッグスタート
		win->MouseDragStart(button,mx,my,MOUSEDRAGFLAGS_DONTHIDEMOUSE|MOUSEDRAGFLAGS_NOMOVE);
		while (win->MouseDrag(&dx,&dy,&device)==MOUSEDRAGRESULT_CONTINUE)		//ドラッグ距離の取得
		{

				bc=BaseContainer();
				win->BfGetInputEvent(BFM_INPUT_MOUSE,&bc);
				#ifdef __PC
						{
						if (dx == 0.0 && dy == 0.0) continue;
						}
				#else
						{
							if (dx < 1 && dx >-1  && dy < 1 && dy >-1 ) continue;
						}
				#endif
				mx+=dx;
				my+=dy;

				G_markpoint=bd->SW(Vector(mx,my,500));

				Vector MenuV2=Vector(mx,my,0);

				MenuV2=MenuV2-MenuV1;
				Matrix m0;
				Float rad=DP.VectorToRad(MenuV2, Vector(1,0,0), m0);

				rad=Deg(rad);
				Float LS=Len(MenuV2);

				if(rad < 45 && rad >-45 && LS > 50)
				{
					MenuDrag =1;
				}
				else if(rad < 135 && rad >45 && LS > 50)
				{
					MenuDrag =2;
				}
				else if(rad > -135 && rad <-45 && LS > 50) //Move Mode
				{
					MenuDrag =3;
				}
				else if(LS > 50)  //Offset Setting
				{
					MenuDrag =4;
				}
				else
				{
					MenuDrag =0;

				}
				DragPos=Vector(mx,my,0);

				DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);

		}
		if (win->MouseDragEnd()==MOUSEDRAGRESULT_FINISHED)
		{

			switch(MenuDrag)
			{
			case 1: //スナップ設定パネルオープン
				//if (AxisOnFlag && data.GetBool(SNAP_ENABLEG))
				//{
				//	BaseContainer bbc = SnapSettings(doc);
				//	bbc.SetBool(SNAP_SETTINGS_ENABLED, true);
				//	SnapSettings(doc, bbc);
				//}
				Sndlg.SetFlag(&SnpDialogFlag);
				Sndlg.Open(DLG_TYPE_ASYNC_POPUP_RESIZEABLE, ID_MODELING_SnapALIGN, (Int32)mx, (Int32)my, 0, 0, 0);

				MenuOpen1=1;
				break;

			case 2:
				data.SetInt32(MDATA_SNAP_MODE, MDATA_CH_ALIGN);
				break;
			case 3:
				data.SetInt32(MDATA_SNAP_MODE, MDATA_CH_POSFIX);
				break;
			case 4:

				//オプションパネルオープン	
				dlg.Open(DLG_TYPE_ASYNC_POPUP_RESIZEABLE, ID_MODELING_SnapALIGN, (Int32)mx, (Int32)my, 0, 0, 1);
				MenuOpen1=4;
				break;
			}

			MenuDrag=-1;
		}
	}
	else
	{
	
		//基準平面設定モード
		if(PlaneSet_BTNFlag)
		{	
			
			if(G_SnapFlag )
			{		
				//**********************************************
				if(!PlaneSetCnt)
				{
				
					if( G_SnapFlag == 1 ) //頂点スナップ
					{
						PlV[0]=G_markpoint;
						PlaneSetCnt++;
						
					}
					else if( G_SnapFlag == 2 )
					{
						//Vector n = MatrixToHPB(EndMg, ROTATIONORDER_DEFAULT);
						//DP.printcheckR("n=", n);


						BaseObject* plane= GetWorkplaneObject( doc);
						Matrix pm=EndMg;
						pm.off=G_markpoint;
						plane->SetMg(pm);
						PlaneSetCnt=0;
						PlaneSet_Flag=true;
						PlaneSet_BTNFlag=false;
						
					}
				}
				else if(PlaneSetCnt==1)
				{
					if( G_SnapFlag == 1 )
					{
						PlV[1]= G_markpoint;
						PlaneSetCnt++;
					}
				}
				else if(PlaneSetCnt==2)
				{
					if( G_SnapFlag == 1 )
					{
						PlV[2]=G_markpoint;
						if (!DP.Get3PointNormMg(PlV[0], PlV[1], PlV[2])) return  ResetPlaneAxis(doc, data);

						BaseObject* plane = GetWorkplaneObject(doc);
						EndMg = plane->GetMg();
						PlaneSetCnt=0;
						PlaneSet_Flag=true;
						PlaneSet_BTNFlag=false;
					}
				}
			}
			//else
			//{
			//	EndMg = FaceMg;

			//}
		//**********************************************
		}
        else if(majer && G_SnapFlag) //測定オブジェクトを追加
        {

			BaseObject* root=nullptr;
			BaseObject* first = doc->GetFirstObject();
			if(first)
			{
				DP.SerchiOBJ(first, &root);
				if(!root)
				{		
					root=BaseObject::Alloc(Onull);
					if(!root) return false;
					root->SetName("##Measure_root##");
					doc->InsertObject(root,NULL,NULL);
					doc->AddUndo(UNDOTYPE_NEW,  root);
				}
			}
			else
			{
			  root=BaseObject::Alloc(Onull);
			  if(!root) return false;
			  root->SetName("##Measure_root##");
			  doc->InsertObject(root,NULL,NULL);
			  doc->AddUndo(UNDOTYPE_NEW,  root);

			}

			BaseObject*MEASURE=BaseObject::Alloc(ID_MEASURE_OBJECT);
			if(!MEASURE)
			{
				ResetTool();
				return false;
			}
			doc->AddUndo(UNDOTYPE_NEW,  MEASURE);

			doc->InsertObject(MEASURE,root,NULL);

			BaseContainer* bc=MEASURE->GetDataInstance();

            GeListHead *layerlist = NULL;
            layerlist = doc->GetLayerObjectRoot();
            LayerObject *layer = (LayerObject*)layerlist->GetFirst();
            LayerObject *layerS=nullptr;
            if(layer)
            {
                Bool flag=false;
               
                
                DP.GetLayerListdata( layer, &flag, &layerS);
                if(!flag)
                {
                    layerS= LayerObject::Alloc();
                    if (!layerS) return false;
					doc->AddUndo(UNDOTYPE_NEW,  layerS);
                    layerS->SetName("Measure");
                    layerlist->Insert( layerS , NULL, NULL);
                    layerlist->Message(MSG_UPDATE); 
                }
 
            }
            else
            {
                
                layerS= LayerObject::Alloc();
                if (!layerS) return false;
                layerS->SetName("Measure");
                layerlist->Insert( layerS , NULL, NULL);
                layerlist->Message(MSG_UPDATE);
            }
            MEASURE->SetLayerObject(layerS);
            MEASURE->Message(MSG_UPDATE);
			root->SetLayerObject(layerS);
            
            //オブジェクトマネージャーで非表示
            LayerData ldata(*layerS->GetLayerData(doc, true));
            ldata.manager=false;
            layerS->SetLayerData(doc,ldata );
            layerS->Message(MSG_UPDATE);
          
			
			 //全てオンの場合
			if(data.GetBool(  MDATA_SNAP_AX) && data.GetBool(  MDATA_SNAP_AY) && data.GetBool(  MDATA_SNAP_AZ) )
			{
				bc->SetBool(MDATA_MEASURE_3RD_POINT, false);
				bc->SetBool(MDATA_MEASURE_SHOWDIST1, true);
				bc->SetBool(MDATA_MEASURE_SHOWDIST2, true);

				////1点目設定
				if(_Starget2) bc->SetLink( MDATA_MEASURE_PNT1_LINK ,_Starget2);
				bc->SetVector(MDATA_MEASURE_PNT1_VECTOR, G_P1);
				Int32 n=0;
				if(SnapID2!=-1 && _Starget2 && ( _Starget2->GetType() ==Opolygon || _Starget2->GetType() ==Ospline) )
				{
					if(G_SnapFlag==1)n=1;
					else if( G_SnapFlag==2)n=3;
					else if( G_SnapFlag==3)n=2;
				
				}
				bc->SetInt32(MDATA_MEASURE_PNT1_MODE, n);
				bc->SetInt32(MDATA_MEASURE_PNT1_IDX,SnapID2);


				//２点目設定
				n=0;//worldに設定

				//スナップ有効＆スナップ先がポリゴンまたはスプラインの場合は
  				if(SnapID1!=-1 && _Starget1 && ( _Starget1->GetType() ==Opolygon || _Starget1->GetType() ==Ospline) )
				{
					//スナップモード（poinntt,edge, polygon）に設定
					if(G_SnapFlag==1)n=1;
					else if( G_SnapFlag==2)n=3;
					else if( G_SnapFlag==3)n=2;
				}
				else
				{
					SnapID1=-1;	 //-1に設定
				}
				if(_Starget1) bc->SetLink( MDATA_MEASURE_PNT2_LINK ,_Starget1);
				bc->SetInt32(MDATA_MEASURE_PNT2_MODE, n); //スナップモード設定
				bc->SetInt32(MDATA_MEASURE_PNT2_IDX,SnapID1); 	 //スナップ先IDを設定
				bc->SetVector(MDATA_MEASURE_PNT2_VECTOR, Get2);	 //スナップ座標を設定
			}
			else //XYZの１つでもオフの場合
			{
                bc->SetBool(MDATA_MEASURE_3RD_POINT, true);	//３ポイントモード有効
				bc->SetBool(MDATA_MEASURE_SHOWDIST1, true);
				bc->SetBool(MDATA_MEASURE_SHOWDIST2, true);
                bc->SetBool(MDATA_MEASURE_SHOWANGLE, true);
                
				SnapID1=-1;
				Int32 n=0;
                ////1点目設定
				if(_Starget2) bc->SetLink( MDATA_MEASURE_PNT1_LINK ,_Starget2);
				bc->SetVector(MDATA_MEASURE_PNT2_VECTOR, G_P1);
				
				if(SnapID2!=-1 && _Starget2 && ( _Starget2->GetType() ==Opolygon || _Starget2->GetType() ==Ospline) )
				{
					if(G_SnapFlag==1)n=1;
					else if( G_SnapFlag==2)n=3;
					else if( G_SnapFlag==3)n=2;
                    
				}
				bc->SetInt32(MDATA_MEASURE_PNT2_MODE, n);
				bc->SetInt32(MDATA_MEASURE_PNT2_IDX,SnapID2);
                bc->SetVector(MDATA_MEASURE_PNT2_VECTOR, G_P1);	 //スナップ座標を設定
                
                
				//２点目設定
				n=0;//worldに設定
                
				//スナップ有効＆スナップ先がポリゴンまたはスプラインの場合は
  				if(SnapID1!=-1 && _Starget1 && ( _Starget1->GetType() ==Opolygon || _Starget1->GetType() ==Ospline) )
				{
					//スナップモード（poinntt,edge, polygon）に設定
					if(G_SnapFlag==1)n=1;
					else if( G_SnapFlag==2)n=3;
					else if( G_SnapFlag==3)n=2;
				}
				else
				{
					SnapID1=-1;	 //-1に設定
				}
				if(_Starget1) bc->SetLink( MDATA_MEASURE_PNT3_LINK ,_Starget1);
				bc->SetInt32(MDATA_MEASURE_PNT3_MODE, n); //スナップモード設定
				bc->SetInt32(MDATA_MEASURE_PNT3_IDX,SnapID1); 	 //スナップ先IDを設定
				bc->SetVector(MDATA_MEASURE_PNT3_VECTOR, G_markpoint);	 //スナップ座標を設定
                
                
  				bc->SetLink( MDATA_MEASURE_PNT1_LINK ,nullptr);
				bc->SetInt32(MDATA_MEASURE_PNT1_MODE, 0); //スナップモード設定
				bc->SetInt32(MDATA_MEASURE_PNT1_IDX,-1); 	 //スナップ先IDを設定
				bc->SetVector(MDATA_MEASURE_PNT1_VECTOR, Get2 );	 //スナップ座標を設定              
			}
			 MEASURE->SetName("Measure-Target="+_Starget2->GetName());


            EventAdd();
			ResetTool();
			return true;
        }
		else
		{
			if (G_SnapFlag == 0) return false;
			Float dx,dy;

			BaseContainer bc;
			BaseContainer device;

			Int32 view = doc->GetActiveBaseDraw()->GetProjection();
		//	G_SnapFlag = 0;
			//G_markpoint = bd->SW(Vector(mx, my, 500));
			if (!_snap)
			{
				_snap = SnapCore::Alloc();
				if (!_snap)	 return false;
			}
			if (!_snap->Init(doc, bd)) return false;
			if (_snap->Snap(G_markpoint, snapResul, SNAPFLAGS_0) && snapResul.snapmode != NOTOK)
			{
				G_SnapFlag = 1;
				G_markpoint = snapResul.mat.off;

				//オルソビュー時は２Dスナップ座標を取得
				if (doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
				{
					G_markpoint = snapResul.initial_snap;
				}
				Mp1=G_markpoint;	//ドラッグファースト座標取得
		
			}

			// use snap matrix for inferred guide to use the correct orientation
			if (snapResul.snapmode != NOTOK)
				guideMatrix = snapResul.mat;
			else
				guideMatrix.off = G_markpoint;

			_snap->AddInferred(doc, guideMatrix, INFERRED_GUIDE_POINT);


			////マウスドラッグスタート
			win->MouseDragStart(button,mx,my,MOUSEDRAGFLAGS_DONTHIDEMOUSE|MOUSEDRAGFLAGS_NOMOVE);
			while (win->MouseDrag(&dx,&dy,&device)==MOUSEDRAGRESULT_CONTINUE)		//ドラッグ距離の取得
			{
				if(majer==0)
				{
					bc=BaseContainer();
					win->BfGetInputEvent(BFM_INPUT_MOUSE,&bc);
        
                    #ifdef __PC
                    {
                        if (dx == 0.0 && dy == 0.0) continue;
                    }
                    #else
                    {
                        if (dx < 1 && dx >-1  && dy < 1 && dy >-1 ) continue;
                    }
                    #endif
                    
					mx+=dx;
					my+=dy;

					Drag = true;

					G_markpoint = bd->SW(Vector(mx, my, bd->WS(G_markpoint).z));
					snapResul = SnapResult();

				
					if (_snap->Snap(G_markpoint, snapResul, SNAPFLAGS_0) && snapResul.snapmode != NOTOK)
					{
						G_markpoint = snapResul.mat.off;
						if (SNAPMODE_WORKPLANE != snapResul.snapmode) G_SnapFlag = 5;
						else if (snapResul.snapmode == SNAPMODE_POINT) G_SnapFlag = 1;
						else if (snapResul.snapmode == SNAPMODE_POLYGON)G_SnapFlag = 2;
						else if (snapResul.snapmode == SNAPMODE_MIDPOINT || snapResul.snapmode == SNAPMODE_POLYGONCENTER) G_SnapFlag = 3;
						else if (snapResul.snapmode == SNAPMODE_EDGE) G_SnapFlag = 4;
						else G_SnapFlag = 0;

						if (view == 6 || view == 7) //top&bottom
						{
							if (NextFlag == 1)
							{
								G_markpoint.y = Mp1.y;
							}
							else
							{
								G_markpoint.y = 0;
							}
						}
						else if (view == 4 || view == 5) //front &back
						{
							if (NextFlag == 1)
							{
								G_markpoint.z = Mp1.z;
							}
							else
							{
								G_markpoint.z = 0;
							}
						}
						else if (view == 3 || view == 2) //right&left
						{
							if (NextFlag == 1)
							{
								G_markpoint.x = Mp1.x;
							}
							else
							{
								G_markpoint.x = 0;
							}
						}
					}
					else
					{

						G_SnapFlag = 0;
						//MMP=bd->SW(Vector(mx,my,500));
						if (view == 6 || view == 7) //top&bottom
						{
							G_markpoint.y = Mp1.y;
							//	MMP.y = G_P1.y;
						}
						else if (view == 4 || view == 5) //front &back
						{
							G_markpoint.z = Mp1.z;
							//MMP.z=0;
						}
						else if (view == 3 || view == 2) //right&left
						{
							G_markpoint.x = Mp1.x;
							//MMP.x=0;
						}
						MMP = G_markpoint;
						//Alignモード 選択ﾎﾟｲﾝﾄの中心を求める
						//if (data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_ALIGN)
						//{
						//	PointCenter(doc);
						//}

					}

					//printcheck("-------------G_markpoint=", G_markpoint);
					DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
				}
			}
			//マウスフィニッシュ
			if (win->MouseDragEnd()==MOUSEDRAGRESULT_FINISHED)
			{
				if(majer==0)
				{
					//Alignモード ノーマルモード
					if(data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_ALIGN )
					{
						//Linerモード
						if(data.GetBool(MDATA_BOOL_LINEALIGN))
						{
							//最初のクリック
							if(!NextFlag)
							{
								if(G_SnapFlag)
								{
									if(!GetModelInfo(doc))
									{
										ResetTool();
										return false;
									}
										//goto Error;
									G_P1=G_markpoint;
									NextFlag=1;
								}
							}
							else if( NextFlag == 1 ) //２点目のクリック
							{
								if(G_SnapFlag)
								{
									if(!SetAlign(doc, data))
									{
										ResetTool();
										return false;
									}
									ResetTool();
								}
							}
						}
						else
						{
							if(CutSnapFlag == 2 )
							{
								//ﾎﾟｲﾝﾄID＆ポリIDを取得
								if (data.GetBool(MDATA_BOOL_IDE_UVFIX))
								{
									DP.GetObjectFace(doc, bd, data, mx, my, &ID_POLY, &ID_POINT);
								}
								if(!SetAlign(doc, data))
								{
									ResetTool();
									return false;
								}
								CutSnapFlag=0;
							}
							else if(CutSnapFlag == 0)
							{
								Int32 cutCnt=data.GetInt32(SNAP_SECTION ); //分割数取得

								if (G_SnapFlag) //スナップしている場合のみ処理
								{

									if(cutCnt>2 && Drag)   //分割数が3以上でドラッグした場合分割指定モード有効
									{
										  CutSnapFlag=1;
										  NextFlag=2;
									}
									else
									{
										//ﾎﾟｲﾝﾄID＆ポリIDを取得
										if (data.GetBool(MDATA_BOOL_IDE_UVFIX))
										{
											DP.GetObjectFace(doc, bd, data, mx, my, &ID_POLY, &ID_POINT);
										}
										//整列モード
										if (!SetAlign(doc, data))
										{
											ResetTool();
											return false;
										}
									}
								}
								else
								{
									ResetTool();
									return false;
								}
							}
							
						}
					}
					else if(data.GetInt32(MDATA_SNAP_MODE) == MDATA_CH_POSFIX) //moveモード
					{
						
						//最初のクリック
						if(!NextFlag)
						{
							if (CutSnapFlag == 2)
							{
								G_P1 = G_markpoint;  //1点目の座標を格納
								NextFlag = 1;
								CutSnapFlag = 0;
								if (!GetModelInfo(doc))
								{
									//選択オブジェクトがない場合は計測フラグオン
									majer = 1;
									G_P1 = G_markpoint;
									_Starget2 = _Starget1;
									SnapID2 = SnapID1;
									return false;
								}
							}
							else if (CutSnapFlag == 0) //分割スナップモード０の場合
							{
								Int32 cutCnt = data.GetInt32(SNAP_SECTION); //分割数取得
								if (G_SnapFlag) //スナップ時
								{
									if (cutCnt>2 && Drag)   //分割数が3以上でドラッグした場合分割指定モード有効
									{
										SPANV[0] = Mp1;
										SPANV[1] = G_markpoint;
										CutSnapFlag = 1;
							
									}
									else 
									{
										if (cutCnt == 2 && Drag)
										{
											G_P1 = (G_markpoint + Mp1) / 2;
											CutSnapFlag = 0;
										}
										else
										{
											G_P1 = G_markpoint;  //1点目の座標を格納
										}
										if (!GetModelInfo(doc))
										{
											//選択オブジェクトがない場合は計測フラグオン
											majer = 1;
											G_P1 = G_markpoint;
											_Starget2 = _Starget1;
											SnapID2 = SnapID1;
											return false;
										}

										NextFlag = 1;
									}

								}
								else
								{
									ResetTool();
									return false;
								}
							}

						}
						else if( NextFlag == 1 && (CutSnapFlag == 0 || CutSnapFlag == 2) ) //２点目のクリック
						{
							if(G_SnapFlag)
							{
								//printcheck("           G_markpoint=",G_markpoint);
								if(!SetOffsetMove(doc, data))
								{
									ResetTool();
									return false;
								}
								ResetTool();
							}
						}
					}
					Drag = false;
				}
				else
				{
					if(majer==1 )
					{
						majer=0;
						ResetTool();
					}
				}
			}
		 _snap->FlushInferred();
	
		}
	}
	
	EventAdd();
	return true;
}

////////////////////////////////////////////////////////
//グリッドリセット
Bool SnapAlign::ResetPlaneAxis(BaseDocument* doc, BaseContainer &data)
{

	BaseObject* plane= GetWorkplaneObject( doc);
	Matrix mg;
	plane->SetMg(mg);
	data.SetInt32(IDE_Snap_AXIS, IDE_XZ);
	EndMg=mg;
	PlV[0]=PlV[1]=PlV[2]=Vector();
	PlaneSetCnt=0;
	ResetTool();
	PlaneSet_Flag=false;
	PlaneSet_BTNFlag=false;

	EventAdd();
	return true;
}

////////////////////////////////////////////////////////
//選択ポイント座標を記憶
Bool SnapAlign ::RectPointVector(BaseDocument *doc, BaseObject *op, Int32 i )
{
	if(!op) return false;

	PointObject *PO=NULL;

	if(op->GetType() != Opolygon )
	{
		PO=(PointObject*)op->GetCache();
		if(op->GetType() == Ospline)
		{
			PO=ToSpline(op);
		}

		if(!PO)
		{
			PO=ToPoint(op);
		}
	}
	else
	{
		PO=ToPoint(op);
	}

	mobj[i].Mmg=op->GetMg();

	if(!PO)
	{
		return false;
	}

	if(doc->GetMode() == Medges)
	{
		BaseContainer bc;
		ModelingCommandData cd;
		bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 1);
		bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
		cd.bc=&bc;
		cd.op = op;
		cd.mode=MODELINGCOMMANDMODE_ALL;
		if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
	}
	else if(doc->GetMode() == Mpolygons)
	{
		BaseContainer bc;
		ModelingCommandData cd;
		bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
		bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
		cd.bc=&bc;
		cd.op = op;
		cd.mode=MODELINGCOMMANDMODE_ALL;
		if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
	}

    //選択ポイントを取得
	BaseSelect *bs=PO->GetPointS();
    //選択ポイント数を取得
	Int32 pcnt = bs->GetCount();

    //ポイント座標リストを取得
	Vector *vp=PO->GetPointW();
	if(pcnt)
	{
		mobj[i].scnt=pcnt;

        //選択ポイントの座標を取得
		mobj[i].vv=NewMemClear( Vector, pcnt);
		if(!mobj[i].vv) return false;
		
        //選択ポイントidを取得する配列を作成
		mobj[i].pnum=NewMemClear( Int32, pcnt);
		if(!mobj[i].pnum)
		{
			DeleteMem(mobj[i].vv);
			return false;
		}

		Int32 ndx, seg = 0, smin, smax, i_cnt=0;
		//選択ﾎﾟｲﾝﾄを格納
		while( bs->GetRange(seg++,PO->GetPointCount(), &smin,&smax) )
		{
			for( ndx=smin; ndx<=smax; ndx++ )
			{
				mobj[i].vv[i_cnt]=vp[ndx]; //頂点ベクトルの記憶
				mobj[i].pnum[i_cnt]=ndx;	//ﾎﾟｲﾝﾄ番号の記憶
				i_cnt += 1;
			}
		}
	}

	if(op->GetType() == Opolygon && doc->GetMode() == Mpolygons)
	{

		PolygonObject* Poly=ToPoly(op);
        //選択ポリ取得
		BaseSelect* Pbs=Poly->GetPolygonS();
        //選択ポリゴン数を取得
		mobj[i].Fcnt=Pbs->GetCount();
		//データ領域を確保
		mobj[i].Snum=NewMemClear( Int32, Pbs->GetCount());

		if(!mobj[i].Snum)
		{
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}
        //選択ポリidをSnum格納
		Int32 ndx, seg = 0, smin, smax, cp=0;
		while( Pbs->GetRange(seg++, Poly->GetPolygonCount(), &smin,&smax) )
		{
			for( ndx=smin; ndx<=smax; ndx++ )
			{
				mobj[i].Snum[cp]=ndx;
				cp++;
			}
		}
	}
	return true;
}

///////////////////////////////////////////////////
//Align
Bool SnapAlign ::SetAlign(BaseDocument *doc, BaseContainer &data)
{
	AutoAlloc<AtomArray> arr;
	if (!arr) return false;
	doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	Int32 cnt=arr->GetCount();
	if(cnt == 0) return false;

	

	Float sx= data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
	Float sy= data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
	Float sz= data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);

	AddUndo(doc, arr, UNDOTYPE_CHANGE);
	Int32 mode=doc->GetMode();

	Vector norm= Vector();
	BaseContainer bbc=doc->GetData(DOCUMENTSETTINGS_DOCUMENT);

	DTOOL DP;
	for(Int32 i=0; i<cnt; i++)
	{
		BaseObject *op=(BaseObject *)arr->GetIndex(i);
		if(op)
		{
				 
			//各エレメントモードの場合
			if( mode== Mpoints || mode == Medges || mode == Mpolygons )
			{
				if(op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd)
				{

					if(doc->GetMode() == Medges && op->GetType() == Opolygon)
					{
						//shiftキーが押されたらポリ作成
						if(PShift)
						{
							BaseContainer bc2;
							ModelingCommandData cd2;
							bc2.SetFloat( MDATA_EXTRUDE_OFFSET, 0);
							bc2.SetBool( MDATA_EXTRUDE_PRESERVEGROUPS, true);
							bc2.SetInt32(MDATA_EXTRUDE_SUBDIVISION, 0);
							bc2.SetBool(MDATA_EXTRUDE_CREATENGONS, false);
							bc2.SetFloat( MDATA_EXTRUDE_EDGEANGLE, 0.0);
							cd2.bc=&bc2;
							cd2.op = op;
							cd2.mode=MODELINGCOMMANDMODE_EDGESELECTION;
							if(!SendModelingCommand(ID_MODELING_EXTRUDE_TOOL, cd2)) return false;
						}

						BaseContainer bc;
						ModelingCommandData cd;
						bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 1);
						bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
						cd.bc=&bc;
						cd.op = op;
						cd.mode=MODELINGCOMMANDMODE_ALL;
						if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
					}
					else if(doc->GetMode() == Mpolygons && op->GetType() == Opolygon)
					{
						//shiftキーが押されたらポリ作成
						if(PShift)
						{
							BaseContainer bc2;
							ModelingCommandData cd2;
							bc2.SetFloat( MDATA_EXTRUDE_OFFSET, 0);
							bc2.SetBool( MDATA_EXTRUDE_PRESERVEGROUPS, true);
							bc2.SetInt32(MDATA_EXTRUDE_SUBDIVISION, 0);
							bc2.SetBool(MDATA_EXTRUDE_CREATENGONS, false);
							bc2.SetFloat( MDATA_EXTRUDE_EDGEANGLE, 0.0);
							cd2.bc=&bc2;
							cd2.op = op;
							cd2.mode=MODELINGCOMMANDMODE_POLYGONSELECTION;
							if(!SendModelingCommand(ID_MODELING_EXTRUDE_TOOL, cd2)) return false;


						}
						BaseContainer bc;
						ModelingCommandData cd;
						bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
						bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
						cd.bc=&bc;
						cd.op = op;
						cd.mode=MODELINGCOMMANDMODE_ALL;
						if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
					}
					//GePrint(op->GetName());
					PointObject *pp=NULL;
					//SplineObject *sop=NULL;
					Vector *Cvp;
					BaseSelect *bs;

					pp=ToPoint(op);
					if(!pp) return false;
					Cvp=pp->GetPointW();
					if(!Cvp) return false;
					bs=pp->GetPointS();


					Int32 ndx, seg = 0, smin, smax;
					Matrix mg=op->GetMg();
					Matrix mmg=op->GetMg();
					mmg.off=Vector(0,0,0);
					Vector tgp;

					//ドラッグ中は中間の座標を取得
					if(Drag)
					{
						tgp=(G_markpoint+Mp1)/2;	//中間座標
					}
					else
					{
						tgp=G_markpoint;
					}

					//ラインモード
					if(data.GetBool(MDATA_BOOL_LINEALIGN))
					{
						norm=tgp-G_P1;
						norm.Normalize();
					}
					Matrix Smg=EndMg;
					Smg.off=Vector();
					//tgp=!mg*tgp; //ローカル座標変換

					if(UV_Sw && op->GetType() == Opolygon )
					{
						//UVポイント修正
						if( data.GetBool(MDATA_BOOL_IDE_UVFIX))
						{
							if(op->GetType() == Opolygon)
							{
								//UVタグを取得
								UVWTag *m_uvTag=(UVWTag*)op->GetTag(Tuvw);
								if(m_uvTag)
								{
									if(!SetUVPointFix(doc,op,m_uvTag)) return false;
								}
							}
						}
					}

					if(op->GetType() == Ospline)
					{
						while( bs->GetRange(seg++, pp->GetPointCount(), &smin,&smax) )
						{
							for( ndx=smin; ndx<=smax; ndx++ )
							{
								Vector Glp=mg*Cvp[ndx]; //ﾎﾟｲﾝﾄのグローバル化
								Vector vx= Vector();
							
								if(data.GetBool(MDATA_BOOL_LINEALIGN))
								{
									vx=PointLineDistance(G_P1,norm,Glp);
									vx*=-1;
								}
								else
								{
									vx=	tgp-Glp;
								}						
								vx=~Smg*vx;
								Vector vc=Vector(0,0,0);
								if(data.GetBool(MDATA_SNAP_AX))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.x=vx.x+sx;
									}
									vc=vc+Vector(vx.x,0,0);
								}
								if(data.GetBool(MDATA_SNAP_AY))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.y=vx.y+sy;
									}
									vc=vc+Vector(0,vx.y,0);
								}
								if(data.GetBool(MDATA_SNAP_AZ))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.z=vx.z+sz;
									}
									vc=vc+Vector(0,0,vx.z);
								}
								vc=Smg*vc;
								vc=~mmg*vc;
								Cvp[ndx]+=vc;
							}
						}
						op->Message(MSG_UPDATE);			
					}
					else if(op->GetType() == Opolygon )
					{

						if (mode == Medges &&data.GetBool(MDATA_BOOL_LINEALIGN))
						{
							PolygonObject* Poly = ToPoly(op);
							BaseSelect* Ebs = Poly->GetEdgeS();
							Int32 ecnt = Ebs->GetCount();
							const CPolygon*  poly = Poly->GetPolygonR(); //ポリゴンリスト取得
							if (ecnt)
							{
								AutoAlloc<Modeling> mod;
								if (!mod || !mod->InitObject(op)) return false;
								while (Ebs->GetRange(seg++, Poly->GetPolygonCount() * 4, &smin, &smax))
								{
									for (ndx = smin; ndx <= smax; ndx++)
									{
										Int32 Id_e = ndx % 4; //エッジインデックスを取得
										Int32 polyNdx = (ndx - Id_e) / 4; //ポリゴンIDを取得

										Int32 a, b;
										a = NULL;
										b = NULL;
										//エッジにつながるポイントインデックス取得
										switch (Id_e)
										{
										case 0: a = poly[polyNdx].a; b = poly[polyNdx].b;  break;
										case 1: a = poly[polyNdx].b; b = poly[polyNdx].c;  break;
										case 2: a = poly[polyNdx].c; b = poly[polyNdx].d;  break;
										case 3: a = poly[polyNdx].d; b = poly[polyNdx].a;  break;
										}

										//GePrint("a=" + String::IntToString(a));
										//GePrint("b=" + String::IntToString(b));
										Vector va = mg*Cvp[a];
										Vector vb = mg*Cvp[b];
										//交点を求める
										Vector pv1, pv2;
										DP.CrossSectionB(G_P1, G_markpoint, va, vb, &pv1, &pv2, data, EndMg);
										//交点に近いエッジポイントを取得
										Vector vt;
										Int32 vid;
										if (Len(pv2 - va) < Len(pv2 - vb))
										{
											vt = va;
											vid = a;
										}
										else
										{
											vt = vb;
											vid = b;
										}
										//エッジポイントと交点の差を取得
										Vector leg = pv2 - vt;
										//基準平面にローカル化
										Vector vx = ~Smg*leg;
										Vector vc = Vector(0, 0, 0);
										if (data.GetBool(MDATA_SNAP_AX))
										{
											if (data.GetBool(MDATA_BOOL_OFFSET))
											{

												vx.x = vx.x + sx;
											}
											vc = vc + Vector(vx.x, 0, 0);
										}
										if (data.GetBool(MDATA_SNAP_AY))
										{
											if (data.GetBool(MDATA_BOOL_OFFSET))
											{

												vx.y = vx.y + sy;
											}
											vc = vc + Vector(0, vx.y, 0);
										}
										if (data.GetBool(MDATA_SNAP_AZ))
										{
											if (data.GetBool(MDATA_BOOL_OFFSET))
											{

												vx.z = vx.z + sz;
											}
											vc = vc + Vector(0, 0, vx.z);
										}
										vc = Smg*vc;
										pv2 = vt + vc;
										pv2=~mg*pv2;
										if (!mod->SetPoint(op, vid, pv2, MODELING_SETPOINT_FLAG_EMPTY)) return false;
									}
								}
								if (!mod->Commit(op, MODELING_COMMIT_UPDATE)) return false;
							}
						}
						else
						{
							AutoAlloc<Modeling> mod;
							if (!mod || !mod->InitObject(op)) return false;

							while (bs->GetRange(seg++, pp->GetPointCount(), &smin, &smax))
							{
								for (ndx = smin; ndx <= smax; ndx++)
								{
									Vector Glp = mg*Cvp[ndx]; //ﾎﾟｲﾝﾄのグローバル化
									Vector vx = Vector();

									if (data.GetBool(MDATA_BOOL_LINEALIGN))
									{
										vx = PointLineDistance(G_P1, norm, Glp);
										vx *= -1;
									}
									else
									{
										vx = tgp - Glp;
									}
									vx = ~Smg*vx;
									Vector vc = Vector(0, 0, 0);
									if (data.GetBool(MDATA_SNAP_AX))
									{
										if (data.GetBool(MDATA_BOOL_OFFSET))
										{
											vx.x = vx.x + sx;
										}
										vc = vc + Vector(vx.x, 0, 0);
									}
									if (data.GetBool(MDATA_SNAP_AY))
									{
										if (data.GetBool(MDATA_BOOL_OFFSET))
										{
											vx.y = vx.y + sy;
										}
										vc = vc + Vector(0, vx.y, 0);
									}
									if (data.GetBool(MDATA_SNAP_AZ))
									{
										if (data.GetBool(MDATA_BOOL_OFFSET))
										{
											vx.z = vx.z + sz;
										}
										vc = vc + Vector(0, 0, vx.z);
									}
									vc = Smg*vc;
									vc = ~mmg*vc;
									if (!mod->SetPoint(op, ndx, Cvp[ndx] + vc, MODELING_SETPOINT_FLAG_EMPTY)) return false;
								}
							}
							if (!mod->Commit(op, MODELING_COMMIT_UPDATE)) return false;
						}
					}
					else if( op->GetType() == Offd )
					{
						while( bs->GetRange(seg++,pp->GetPointCount(), &smin,&smax) )
						{
							for( ndx=smin; ndx<=smax; ndx++ )
							{
								Vector Glp=mg*Cvp[ndx]; //ﾎﾟｲﾝﾄのグローバル化
								Vector vx=Vector();

								if(data.GetBool(MDATA_BOOL_LINEALIGN))
								{
									vx=PointLineDistance(G_P1,norm,Glp);
									vx*=-1;
								}
								else
								{
									vx=	tgp-Glp;
								}
								vx=~Smg*vx;
								Vector vc=Vector(0,0,0);
								if(data.GetBool(MDATA_SNAP_AX))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.x=vx.x+sx;
									}
									vc=vc+Vector(vx.x,0,0);
								}
								if(data.GetBool(MDATA_SNAP_AY))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.y=vx.y+sy;
									}
									vc=vc+Vector(0,vx.y,0);
								}
								if(data.GetBool(MDATA_SNAP_AZ))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										vx.z=vx.z+sz;
									}
									vc=vc+Vector(0,0,vx.z);
								}
								vc=Smg*vc;
								vc=~mmg*vc;

								Cvp[ndx]+=vc;
							}
						}


					}

					if(bs->GetCount())
					{
						//ポイント結合
						if(data.GetBool(MDATA_OPTIMIZE) && op->GetType() != Offd)
						{
							//Int32 cnt;
							const Vector *vp=NULL;

							pp=ToPoint(op);
							cnt=pp->GetPointCount();
							if(!cnt) return false;
							vp=pp->GetPointR();
							if(!vp) return false;

							AutoAlloc<Modeling> mmod;
							if (!mmod || !mmod->InitObject(op)) return false;
							ndx=NULL, seg = 0, smin=NULL, smax=NULL;
							while( bs->GetRange(seg++, pp->GetPointCount(), &smin,&smax) )
							{
								for( ndx=smin; ndx<=smax; ndx++ )
								{
									for(Int32 j=0; j<cnt; j++)
									{
										if(j != ndx )
										{
											if((vp[j]==vp[ndx] || Abs(vp[j].x-vp[ndx].x) < 0.001) && Abs(vp[j].y-vp[ndx].y) < 0.001 && Abs(vp[i].z-vp[ndx].z) < 0.001 )
											{
												if(!mmod->WeldPoints(op,j ,ndx)) return false;
											}
										}
									}
								}
							}
							if (!mmod->Commit(op, MODELING_COMMIT_UPDATE )) return false;
						}
					}
				}
			}
			else if(mode == Mobject || mode == Mmodel )
			{
				
				Vector tgp;
				if(Drag)
				{
					tgp=(G_markpoint+Mp1)/2;
				}
				else
				{
					tgp=G_markpoint;
				}
				Matrix Smg=EndMg;
				Smg.off=Vector();
				Vector Opos=op->GetMg().off;		
				Vector vx;
				Vector vc=Vector(0,0,0);

				Int32 Ccn=0;
				//Matrix64 *cmg=NULL;

				//ラインモード
				if(data.GetBool(MDATA_BOOL_LINEALIGN))
				{
					norm=tgp-G_P1;
					norm.Normalize();
					vx=PointLineDistance(G_P1,norm,Opos);
					vx*=-1;
				}
				else
				{
					vx=tgp-Opos;
				}

				maxon::BaseArray<Matrix>cmg;
				vx=~Smg*vx;
				//子供の移動無しの場合
				if(!data.GetBool(MDATA_CHILD))
				{
					if(op->SearchHierarchy(op))
					{
						BaseObject *cobj=op->GetDown();
						while(cobj)
						{
							Ccn++;
							cobj=cobj->GetNext();
						}
						//cmg = bNew Matrix[Ccn];
						//cmg =new Matrix[Ccn];
						
						//if(!cmg) return false;
						cobj=op->GetDown();
						Ccn=0;
						while(cobj)
						{
							//cmg[Ccn]=cobj->GetMg();
							cmg.Append(	cobj->GetMg());
							Ccn++;
							cobj=cobj->GetNext();
						}
					}
				}
		

				if(data.GetBool(MDATA_SNAP_AX))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						vx.x=vx.x+sx;
					}
					vc=vc+Vector(vx.x,0,0);
				}
				if(data.GetBool(MDATA_SNAP_AY))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						vx.y=vx.y+sy;
					}
					vc=vc+Vector(0,vx.y,0);
				}
				if(data.GetBool(MDATA_SNAP_AZ))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						vx.z=vx.z+sz;
					}
					vc=vc+Vector(0,0,vx.z);
				}

				
				//shiftキーでモデルコピー
				if(PShift)
				{
					BaseObject* Top;
					Top=op;
					if(data.GetBool(MDATA_COPYINS )) //インスタンスモード
					{
						for(Int32 j=1; j<data.GetInt32(MDATA_BOOL_COPYCNT)+1; j++)
						{
							//インスタンスを作成
							BaseObject *ins = NULL;
							ins = BaseObject::Alloc(Oinstance);
							if(!ins) return false;
							AutoAlloc<BaseLink> link;
							if (link)
							{
								link->SetLink(op);
								ins->SetParameter(DescLevel(INSTANCEOBJECT_LINK), GeData(link),DESCFLAGS_SET_0);
							}
							Vector span=Smg*(vc*j);
							Matrix mg=op->GetMg();
							if(data.GetBool(MDATA_NORMALPOSITION ))
							{
								Vector r= MatrixToHPB(Smg, ROTATIONORDER_DEFAULT);
								Matrix rrm=HPBToMatrix(r,ROTATIONORDER_DEFAULT);
								rrm.off=mg.off;
								mg=rrm;
							}
							Vector A1=mg.off;
						
							mg.off=span+A1;
							ins->InsertAfter(Top);


							ins->SetMg(mg);
							ins->SetName(op->GetName()+"_"+String::IntToString(j));
							if (data.GetBool(MDATA_RENDERINS))
							{
								//レンダーインスタンス
								BaseContainer* bc;
								bc = ins->GetDataInstance();
								bc->SetBool(INSTANCEOBJECT_RENDERINSTANCE, true);
							}
							doc->AddUndo(UNDOTYPE_NEW, ins);
							Top=ins;
						}
					}
					else
					{
						for(Int32 j=1; j<data.GetInt32(MDATA_BOOL_COPYCNT)+1; j++)
						{
							BaseObject* copyM=NULL;
							AutoAlloc<AliasTrans> aliastrans;
							if (!aliastrans || !aliastrans->Init(doc)) return false;
						
							if(!data.GetBool(MDATA_CHILD))
							{
								copyM=static_cast<BaseObject*>(op->GetClone(COPYFLAGS_NO_HIERARCHY, aliastrans));
							}
							else
							{
								copyM=static_cast<BaseObject*>(op->GetClone(COPYFLAGS_0, aliastrans));
							}
							aliastrans->Translate(true);
							if(!copyM) return false;

							ObjectColorProperties ocp;

							op->GetColorProperties(&ocp);

							Vector span=Smg*(vc*j);
							Matrix mg=op->GetMg();

							Vector A1=mg.off;
							if(data.GetBool(MDATA_NORMALPOSITION ))
							{
								Vector r= MatrixToHPB(Smg, ROTATIONORDER_DEFAULT);
								Matrix rrm=HPBToMatrix(r,ROTATIONORDER_DEFAULT);
								rrm.off=mg.off;
								mg=rrm;
							}
							mg.off=span+A1;
							copyM->InsertAfter(op);
							copyM->SetMg(mg);

							copyM->SetName(op->GetName()+"_"+String::IntToString(j));
							copyM->SetColorProperties(&ocp);
							copyM->DelBit(BIT_ACTIVE);
	
							doc->AddUndo(UNDOTYPE_NEW, copyM);

						}
					}
				}
				else	//移動モード
				{
				
					if(op->GetType() ==  Oextrude )
					{
						
						 if(!ExFlag )
						 {
							 if( QuestionDialog("Change the size of the Extrude object?") )
							 {
                                 
                                ExFlag=true;
                                BaseContainer* bc=op->GetDataInstance();
                             //   Vector hv=bc->GetVector(EXTRUDEOBJECT_MOVE);
                                BaseObject* ch=op->GetDown();
                                 if(!ch)return false;
                                Vector p=ch->GetMl().off;
                                vc=vc-p;
                                bc->SetVector(EXTRUDEOBJECT_MOVE, vc);
							 }else{

								vc=Smg*vc;
								Matrix Omg=op->GetMg();
								if(data.GetBool(MDATA_NORMALPOSITION ))
								{
									Vector r= MatrixToHPB(Smg, ROTATIONORDER_DEFAULT);
									Matrix rrm=HPBToMatrix(r,ROTATIONORDER_DEFAULT);
									Omg=rrm;
								}
								Omg.off=vc+Opos;
								op->SetMg(Omg);
							 }
						}else {
							BaseContainer* bc=op->GetDataInstance();
                            Vector hv=bc->GetVector(EXTRUDEOBJECT_MOVE);
                            vc+=hv;
							bc->SetVector(EXTRUDEOBJECT_MOVE, vc);
						}
					}else{

						vc=Smg*vc;
						Matrix Omg=op->GetMg();
						if(data.GetBool(MDATA_NORMALPOSITION ))
						{
							Vector r= MatrixToHPB(Smg, ROTATIONORDER_DEFAULT);
							Matrix rrm=HPBToMatrix(r,ROTATIONORDER_DEFAULT);
							Omg=rrm;
						}
						 //printcheck("vc=",vc);
						 //	 printcheck("Opos=",Opos);
							// 	 	 printcheck("vc+Opos=",vc+Opos);
						Omg.off=vc+Opos;

						//------------------------------------------------------------------------------------------
						//アクシズのみ移動
						if(bbc.GetBool(DOCUMENT_AXIS) && (op->GetType() == Opolygon || op->GetType() == Ospline))
						{
							PointObject* pp=ToPoint(op);
							Vector* vp=pp->GetPointW();
							Matrix mg=op->GetMg();
							for( Int32 k=0; k< pp->GetPointCount(); k++)
							{
								Vector pps=mg*vp[k];
								pps=~Omg*pps;
								vp[k]=pps;
							}
						}
 						//------------------------------------------------------------------------------------------
						op->SetMg(Omg);
					}
					op->Message(MSG_UPDATE);
				}
				if(!data.GetBool(MDATA_CHILD))
				{
					if(Ccn)
					{
						Ccn=0;
						BaseObject *cobj=op->GetDown();
						while(cobj)
						{
							cobj->SetMg(cmg[Ccn]);
							Ccn++;
							cobj=cobj->GetNext();
						}
						//bDelete(cmg);
					
						//cmg=nullptr;
					}
				}
				cmg.Flush();
			}
		
		}
	}
	EventAdd();
	
	return true;
}

////////////////////////////////////////////////
//選択モデルの取得
Bool SnapAlign ::GetModelInfo(BaseDocument *doc)
{
	if(!doc) return false;
		////選択オブジェクトの取得
	AutoAlloc <AtomArray> arr;
	if (!doc || !arr) return false;
	//現在のモードを取得
	Int32 mode = doc->GetMode();
	if( mode== Mpoints || mode == Medges || mode == Mpolygons )
	{
		doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
		Int32 cnt=arr->GetCount();
		if(!cnt) return false;
		AddUndo(doc, arr, UNDOTYPE_CHANGE);
		mobj=NULL;				

		Int32 gh=0;

		//選択ポリゴンのみ動くモード
		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *op=(BaseObject *)arr->GetIndex(i);
			if(op && (op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd))
			{
				if(mode == Mpolygons)
				{
					if(op->GetType() == Opolygon)
					{
						PolygonObject* Poly= ToPoly(op);
						BaseSelect* Pbbc=Poly->GetPolygonS();
						if(Pbbc->GetCount()) gh++;
					}
				}
				else if(mode == Medges )
				{
					if(op->GetType() == Opolygon)
					{
						PolygonObject* Poly= ToPoly(op);
						BaseSelect* Pbbc=Poly->GetEdgeS();
						if(Pbbc->GetCount()) gh++;
					}
				}
				else if(mode== Mpoints)
				{
					if(op->GetType() == Opolygon || op->GetType() == Offd)
					{
						PointObject* Pp= ToPoint(op);
						BaseSelect* Pbbc=Pp->GetPointS();
						if(Pbbc->GetCount()) gh++;
					}
					else
					{
						SplineObject* spl= ToSpline(op);
						BaseSelect* Pbbc=spl->GetPointS();
						if(Pbbc->GetCount()) gh++;
					}
				}
			}
		}
			
	
		// 選択モデル分の構造体を作成
		mobj=nullptr;
		mobj=NewMemClear(Mobj, gh );
		if(!mobj) return false;
		gh=0;
		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *op=(BaseObject *)arr->GetIndex(i);
			//if(!op) return false;
			if(op && (op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd))
			{
				if(op->GetType() == Opolygon)
				{
					BaseSelect* Pbbc;
					if( mode== Mpoints )
					{
						PointObject* Pp= ToPoint(op);
						Pbbc=Pp->GetPointS();

					}
					else if( mode== Medges )
					{
						PolygonObject* Poly= ToPoly(op);
						Pbbc=Poly->GetEdgeS();

					}
					else 
					{
						PolygonObject* Poly= ToPoly(op);
						Pbbc=Poly->GetPolygonS();
					}
					if(Pbbc->GetCount())
					{
						mobj[gh].op=op;	//オブジェクトを格納
						mobj[gh].Mmg=op->GetMg(); //オブジェクトのマトリックスを格納
						if(!RectPointVector(doc, op, gh))
						{
							ResetTool();
							return false;
						}
						gh++;
					}
				}
				else  if(op->GetType() == Ospline)
				{
					if( mode== Mpoints )
					{
						SplineObject* spl= ToSpline(op);
						BaseSelect* Pbbc=spl->GetPointS();
						if(Pbbc->GetCount())
						{
								mobj[gh].op=op;	 //オブジェクトを格納
							mobj[gh].Mmg=op->GetMg(); //オブジェクトのマトリックスを格納
							if(!RectPointVector(doc, op, gh))
							{
								ResetTool();
								return false;
							}
							gh++;
						}
					}
				}
				else if( op->GetType() == Offd && mode== Mpoints)
				{
 					BaseSelect* Pbbc;
					PointObject* Pp= ToPoint(op);
					Pbbc=Pp->GetPointS();
					if(Pbbc->GetCount())
					{
						mobj[gh].op=op;
						mobj[gh].Mmg=op->GetMg();
						if(!RectPointVector(doc, op, gh))
						{
							ResetTool();
							return false;
						}
						gh++;
					}
				}
			}
		}
		ObjCnt=gh;
	}
	else if(mode == Mobject || mode == Mmodel )
	{
//		GePrint("nnn");
		doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
		Int32 cnt=arr->GetCount();
		if(!cnt) return false;
	
		AddUndo(doc, arr, UNDOTYPE_CHANGE);
		mobj=NULL;	
		//mobj=(Mobj *) GeAlloc(cnt * sizeof(Mobj));
		mobj= NewMemClear(Mobj, cnt);
		if(!mobj) return false;

		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *op=(BaseObject *)arr->GetIndex(i);
			if(!op) return false;
			mobj[i].op=op;
			mobj[i].Mmg=op->GetMg();
		}
		ObjCnt=cnt;
	}
	return true;
}


////////////////////////////////////////////
//選択ﾎﾟｲﾝﾄの中心を求める
Bool SnapAlign::PointCenter(BaseDocument *doc)
{
	if(!doc) return false;

	AutoAlloc<AtomArray> arr;
	if (!arr) return false;
	doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	Int32 cnt=arr->GetCount();
	if(!cnt) return false;

	Int32 SetCnt=0;
	Vector Vall=Vector();
	Int32 mode =doc->GetMode();
	for(Int32 i=0; i<cnt; i++)
	{
		BaseObject *op=(BaseObject *)arr->GetIndex(i);
		if(!op) return false;
		if( mode== Mpoints || mode == Medges || mode == Mpolygons )
		{
			if(op->GetType() == Opolygon || op->GetType() == Ospline)
			{
				if(doc->GetMode() == Medges)
				{
					BaseContainer bc;
					ModelingCommandData cd;
					bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 1);
					bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
					cd.bc=&bc;
					cd.op = op;
					cd.mode=MODELINGCOMMANDMODE_ALL;
					if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
				}
				else if(doc->GetMode() == Mpolygons)
				{
					BaseContainer bc;
					ModelingCommandData cd;
					bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
					bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
					cd.bc=&bc;
					cd.op = op;
					cd.mode=MODELINGCOMMANDMODE_ALL;
					if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
				}

				PointObject *pp=ToPoint(op);
				const Vector *Cvp=pp->GetPointR();
				BaseSelect *bs=pp->GetPointS();

				Int32 SelCnt=bs->GetCount();
				SetCnt=SetCnt+SelCnt;
				Int32 ndx, seg = 0, smin, smax;
				Matrix mg=op->GetMg();
				while( bs->GetRange(seg++, pp->GetPointCount(), &smin,&smax) )
				{
					for( ndx=smin; ndx<=smax; ndx++ )
					{
						Vector Pre=Cvp[ndx]; //ローカル座標
						Pre=mg*Pre;	//グローバル座標
						Vall=Vall+Pre;
					}
				}
			}
		}
		else if(mode == Mobject || mode == Mmodel )
		{
			Matrix cmg=op->GetMg();
			Vall=Vall+cmg.off;
		}
	}

	if( mode== Mpoints || mode == Medges || mode == Mpolygons )
	{
		if(!SetCnt) return false;
		Vall=Vall/SetCnt;
	}
	else if(mode == Mobject || mode == Mmodel )
	{
		if(!cnt) return false;
		Vall=Vall/cnt;
	}

	G_markpoint=Vall;
	
	return true;
}

//////////////////////////////////////////////////////////////
//頂点移動
Bool SnapAlign::SetOffsetMove(BaseDocument *doc, BaseContainer &data)
{
	if(!doc) return false;

	if(!ObjCnt) return false;
	
	Float sx= data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
	Float sy= data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
	Float sz= data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);

	Int32 mode=doc->GetMode();


	Matrix Smg=EndMg;
	Smg.off=Vector();
	Vector G2;//２点目の座標
	if(Drag)
	{
		G2=(G_markpoint+Mp1)/2;
	}
	else
	{
		G2=G_markpoint;
	}	


	Vector SetV=G2-G_P1; //１点目と２点目の差を求める
	Vector vx=~Smg*SetV; //ローカル化
	Vector vc=Vector(0,0,0);
	if(data.GetBool(MDATA_SNAP_AX))
	{
		if(data.GetBool(MDATA_BOOL_OFFSET))
		{
			vx.x=vx.x+sx;
		}
		vc=vc+Vector(vx.x,0,0);
	}
	if(data.GetBool(MDATA_SNAP_AY))
	{
		if(data.GetBool(MDATA_BOOL_OFFSET))
		{
			vx.y=vx.y+sy;
		}
		vc=vc+Vector(0,vx.y,0);
	}
	if(data.GetBool(MDATA_SNAP_AZ))
	{
		if(data.GetBool(MDATA_BOOL_OFFSET))
		{
			vx.z=vx.z+sz;
		}
		vc=vc+Vector(0,0,vx.z);
	}
	vc=Smg*vc; //グローバル座標

	doc->StartUndo();

	AutoAlloc <AtomArray> arr;
	if (!doc || !arr) return false;
	doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);

	for(Int32 i=0; i<ObjCnt; i++)
	{

		if( mode== Mpoints || mode == Medges || mode == Mpolygons )
		{
			//Shiftキー併用でポリゴンコピー
			if(PShift &&  mode == Mpolygons)
			{
				 doc->AddUndo(UNDOTYPE_CHANGE, mobj[i].op);
				if(data.GetInt32( MDATA_ACTIONLIST) == 	CH_NONE )
				{
					
					if(!ActionPolyMove( doc, data, mobj[i], vc)) 	//自分自身にポリゴンコピー
					{


					}

				}
				else
				{

					if(_Starget1 && _Starget1->GetType() == Opolygon && _Starget1 != mobj[i].op)
					{
						ActionPolyCutMove(doc, data, mobj[i], vc);
					}
					else
					{
					   ActionPolyMove( doc, data, mobj[i], vc);	//自分自身にポリゴンコピー

					}
				}




			}else if(PShift &&  mode == Medges)
			{
				
  				PointObject *Pobj=ToPoint(mobj[i].op);
				if(!Pobj) return false;
				Int32 Poi = Pobj->GetPointCount();
				  

				BaseContainer bc2;
				ModelingCommandData cd2;
				bc2.SetFloat( MDATA_EXTRUDE_OFFSET, 0);
				bc2.SetBool( MDATA_EXTRUDE_PRESERVEGROUPS, true);
				bc2.SetInt32(MDATA_EXTRUDE_SUBDIVISION, 0);
				bc2.SetBool(MDATA_EXTRUDE_CREATENGONS, false);
				bc2.SetFloat( MDATA_EXTRUDE_EDGEANGLE, 0.0);
				cd2.bc=&bc2;
				cd2.op = mobj[i].op;
				cd2.mode=MODELINGCOMMANDMODE_EDGESELECTION;
				if(!SendModelingCommand(ID_MODELING_EXTRUDE_TOOL, cd2)) return false;			


				

				Int32 tc=  Poi+	mobj[i].scnt;

				Vector *vp=Pobj->GetPointW();
				if(!vp) return false;


				for(Int32 j=0; j< mobj[i].scnt; j++)
				{
					Vector p=mobj[i].vv[j];

					for(Int32 k= Poi; k< tc; k++)
					{
						Vector p2 =vp[k];
						if(  p == p2 )
						{
							p2=mobj[i].Mmg*p2;
							p2=vc+p2;
							p2=~mobj[i].Mmg*p2;
							vp[k]=p2;
						}
					}

				}
				 
				mobj[i].op->Message(MSG_UPDATE);


			}
			else
			{
				PointObject *Pobj=ToPoint(mobj[i].op);
				if(!Pobj) return false;
				Vector *vp=Pobj->GetPointW();
				if(!vp) return false;
				for(Int32 j=0; j< mobj[i].scnt; j++)
				{
					Vector p=mobj[i].vv[j];
					p=mobj[i].Mmg*p;
					p=vc+p;
					p=~mobj[i].Mmg*p;
					vp[mobj[i].pnum[j]]=p;
				}
				mobj[i].op->Message(MSG_UPDATE);
			}

		}
		else if(mode == Mobject || mode == Mmodel )
		{
				
		//	if(data.GetBool(MDATA_BOOL_COPYPOLY)) //コピーモード
			BaseContainer bbc=doc->GetData(DOCUMENTSETTINGS_DOCUMENT);
				
			if(PShift)
			{
				BaseObject* pre=mobj[i].op; 
				if(data.GetBool(MDATA_COPYINS )) //インスタンスモード
				{
					for (Int32 j = 1; j < data.GetInt32(MDATA_BOOL_COPYCNT) + 1; j++)
					{
						Vector off = vc;
						//インスタンスを作成
						BaseObject *ins = NULL;
						ins = BaseObject::Alloc(Oinstance);
						if (!ins) return false;
						AutoAlloc<BaseLink> link;
						if (link)
						{
							link->SetLink(mobj[i].op);
							ins->SetParameter(DescLevel(INSTANCEOBJECT_LINK), GeData(link), DESCFLAGS_SET_0);
						}
						Vector A1 = mobj[i].Mmg.off;
						Matrix mg = mobj[i].Mmg;
						off.x *= j;
						off.y *= j;
						off.z *= j;
						mg.off = off + A1;

						ins->InsertAfter(pre);
						ins->SetMg(mg);
						ins->SetName(mobj[i].op->GetName() + "_" + String::IntToString(j));
						if (data.GetBool(MDATA_RENDERINS))
						{

							//レンダーインスタンス
							BaseContainer* bc;
							bc = ins->GetDataInstance();
							bc->SetBool(INSTANCEOBJECT_RENDERINSTANCE, true);
						}
					
						doc->AddUndo(UNDOTYPE_NEW, ins);
						pre =ins;
					}
				}
				else
				{
					//コピーモード
					for(Int32 j=1; j<data.GetInt32(MDATA_BOOL_COPYCNT)+1; j++)
					{
							
						Vector off=vc;
						BaseObject* copyM=NULL;
						AutoAlloc<AliasTrans> aliastrans;
						if (!aliastrans || !aliastrans->Init(doc)) return false;
							
						if(!data.GetBool(MDATA_CHILD))
						{
							copyM=static_cast<BaseObject*>(mobj[i].op->GetClone(COPYFLAGS_NO_HIERARCHY, aliastrans));
						}
						else
						{
							copyM=static_cast<BaseObject*>(mobj[i].op->GetClone(COPYFLAGS_0, aliastrans));
						}
						aliastrans->Translate(true);
						if(!copyM) return false;
			

						ObjectColorProperties ocp;

						mobj[i].op->GetColorProperties(&ocp);
						//ocp.usecolor =    ID_BASEOBJECT_USECOLOR_ALWAYS;
						//ocp.xray =            true;
						//ocp.color =            Vector(0.0,1.0,0.0); //green
						//pplane->SetColorProperties(&ocp);

						Vector A1=mobj[i].Mmg.off;
						Matrix mg=mobj[i].Mmg;
						off.x*=j;
						off.y*=j;
						off.z*=j;
						Vector VL=off+A1;
						//mg.off=VL.ToLV();
						mg.off=VL;
						copyM->InsertAfter(pre);
						copyM->SetMg(mg);
						copyM->SetName(mobj[i].op->GetName()+"_"+String::IntToString(j));
						copyM->SetColorProperties(&ocp);
						copyM->DelBit(BIT_ACTIVE);
						doc->AddUndo(UNDOTYPE_NEW, copyM);
							pre = copyM;
					}
				}
			}
			else //移動モード
			{
			
				//	extrudeオブジェクトのみ
                if(mobj[i].op->GetType() ==  Oextrude )
                {
                    //op=op->GetDown();
                    if(mobj[i].op)
                    {
                        if(!ExFlag )
                        {
                             if( QuestionDialog("Change the size of the Extrude object?") )
                             {
                                    ExFlag=true;
                                    BaseContainer* bc=mobj[i].op->GetDataInstance();
                                    Vector hv=bc->GetVector(EXTRUDEOBJECT_MOVE);
                                    vc+=hv;
                                    bc->SetVector(EXTRUDEOBJECT_MOVE, vc);

                             }else{

                                Vector A1=mobj[i].Mmg.off;
                                Matrix mg=mobj[i].Mmg;
                                Vector VL=vc+A1;
                                mg.off=VL;
                                mobj[i].op->SetMg(mg);
                            }
                        }
                        else
                        {
                            BaseContainer* bc=mobj[i].op->GetDataInstance();
                            Vector hv=bc->GetVector(EXTRUDEOBJECT_MOVE);
                            vc+=hv;
                            bc->SetVector(EXTRUDEOBJECT_MOVE, vc);
                        }
                    }
                }else{	 //それ以外のオブジェクト
                    Vector A1=mobj[i].Mmg.off;
                    Matrix mg=mobj[i].Mmg;
                    Vector VL=vc+A1;
                    //mg.off=VL.ToLV();
                     mg.off=VL;

					
                    //------------------------------------------------------------------------------------------
                    //アクシズのみ移動
                    if(bbc.GetBool(DOCUMENT_AXIS) && (mobj[i].op->GetType() == Opolygon || mobj[i].op->GetType() == Ospline))
                    {
                        Matrix pmg=mobj[i].Mmg;
                        PointObject* pp=ToPoint(mobj[i].op);
                        Vector* vp=pp->GetPointW();
                    
                        for( Int32 k=0; k< pp->GetPointCount(); k++)
                        {
                            Vector pps=pmg*vp[k];
                            pps=~mg*pps;
                            vp[k]=pps;
                        }
                    }
					Int32 Ccn=0;
					// Matrix *cmg=NULL;
					maxon::BaseArray<Matrix>cmg;
					//子供の移動無しの場合
					if(!data.GetBool(MDATA_CHILD))
					{
						if(mobj[i].op->SearchHierarchy(mobj[i].op))
						{
							BaseObject *cobj=mobj[i].op->GetDown();
							while(cobj)
							{
								Ccn++;
								cobj=cobj->GetNext();
							}
							//cmg = bNew Matrix[Ccn];
							//cmg = new Matrix[Ccn];
						//	if(!cmg) return false;
							cobj=mobj[i].op->GetDown();
							Ccn=0;
							while(cobj)
							{
								//cmg[Ccn]=cobj->GetMg();
								cmg.Append(	cobj->GetMg());
								Ccn++;
								cobj=cobj->GetNext();
							}
						}
					}
					
                    mobj[i].op->SetMg(mg);
					if(!data.GetBool(MDATA_CHILD))
					{
					   if(Ccn > 0)
					   {
							BaseObject *cobj=mobj[i].op->GetDown();
							Int32 cu=0;
							while(cobj)
							{
								cobj->SetMg(cmg[cu]);
								cobj=cobj->GetNext();
								cu++;
							}
						}
					   	//bDelete(cmg);
						//cmg=nullptr;
					}
					cmg.Flush();
                }
                mobj[i].op->Message(MSG_UPDATE);
			}
		}
	}

	doc->EndUndo();
	return true;
}
//////////////////////////////////
//ターゲット先UV座標の取得
Vector SnapAlign ::GetUVvectorID( BaseDocument *doc, BaseObject *op, UVWTag *m_uvTag)
{
	Int32 TuvP=NULL;	//ターゲットUVナンバー
	Vector vuv = Vector(); //ターゲットUV座標
	PolygonObject *Poly=ToPoly(op);
	if(!Poly) return Vector();

	//GePrint("ID_POLY="+Int32ToString(ID_POLY));
	if(ID_POLY)
	{
		if(ID_POINT) //指定ポイントがある場合
		{
//	GePrint("ID_POINT="+Int32ToString(ID_POINT));
			const CPolygon* PP=Poly-> GetPolygonR();
			Int32 num[4];
			num[0]=PP[ID_POLY].a;
			num[1]=PP[ID_POLY].b;
			num[2]=PP[ID_POLY].c;
			num[3]=PP[ID_POLY].d;
			if(num[2] == num[3])
			{
				for(Int32 i=0; i<3; i++)
				{
					if(num[i]==ID_POINT)
					{
						TuvP=i;
					}
				}
			}
			else
			{
				for(Int32 i=0; i<4; i++)
				{
					if(num[i]==ID_POINT)
					{
						TuvP=i;
					}
				}
			}
			UVWStruct Puvw=m_uvTag->GetSlow(ID_POLY);
			switch(TuvP)
			{
			case 0:
				vuv=Puvw.a;
				break;
			case 1:
				vuv=Puvw.b;
				break;
			case 2:
				vuv=Puvw.c;
				break;
			case 3:
				vuv=Puvw.d;
				break;
			}
		}
		else//指定ポイントがない場合
		{
			TempUVHandle *pTempUV = GetActiveUVSet(doc, GETACTIVEUVSET_ALL);
			if( !pTempUV )
			{
				FreeActiveUVSet(pTempUV);
				return Vector();    // Failure
			}
			BaseSelect *pUVSelected = pTempUV->GetUVPointSel();
			//const UVWHandle dataptr = m_uvTag->GetDataAddressR();

			ConstUVWHandle dataptr = m_uvTag->GetDataAddressR();


			Int32 ndx, Pseg = 0, Psmin, Psmax;
			Vector Vget=Vector();
			while( pUVSelected->GetRange(Pseg++,pTempUV->GetPointCount(),  &Psmin,&Psmax) )
			{
				for( ndx=Psmin; ndx<=Psmax; ndx++ )
				{
					UVWStruct uvw;
					Int32 polyNdx;
					polyNdx = ndx >> 2; // first determine which poly this is (divide by 4)
					UVWTag::Get(dataptr, polyNdx, uvw);
					switch( ndx % 4 ) // next determine which point by: ndx mod(4)
					{
						case 0: // do something with uv-point A
								Vget=Vget+uvw.a;
							break;

						case 1: // do something with uv-point B
								Vget=Vget+uvw.b;
							break;

						case 2: // do something with uv-point C
								Vget=Vget+uvw.c;
							break;

						case 3: // do something with uv-point D[]
								Vget=Vget+uvw.d;
								break;
					}
				}
			}
			vuv=Vget/pUVSelected->GetCount();
		}
	}

	return vuv;
}
//////////////////////////////////////////////////////////////////
//ターゲット先のUVポイント座標を求める
Bool SnapAlign ::SetUVPointFix( BaseDocument *doc, BaseObject *op, UVWTag *m_uvTag)
{
	
	if(!doc || !op) return false;
	
//ターゲットのUV座標取得
	if(op->GetType() == Opolygon)
	{
		//ターゲットUV座標の取得
		Vector vuv=GetUVvectorID(doc, op, m_uvTag);
		DTOOL DP;

	//	DTOOL DP;
	//	DP.printcheck("vuv=",vuv);
		TempUVHandle *pTempUV = GetActiveUVSet(doc, GETACTIVEUVSET_ALL);
		if( !pTempUV )
		{
			FreeActiveUVSet(pTempUV);
			return false;    // Failure
		}
		BaseSelect *pUVSelected = pTempUV->GetUVPointSel();
		void *dataptr = m_uvTag->GetDataAddressW();
		Int32 ndx, Pseg = 0, Psmin, Psmax;
		PolygonObject* poly=ToPoly(op);
		while( pUVSelected->GetRange(Pseg++,  poly->GetPolygonCount()*4, &Psmin,&Psmax) )
		{
			for( ndx=Psmin; ndx<=Psmax; ndx++ )
			{
			
				UVWStruct uvw;
				Int32 polyNdx;
				polyNdx = ndx >> 2; // first determine which poly this is (divide by 4)
				UVWTag::Get(dataptr, polyNdx, uvw);
				switch( ndx % 4 ) // next determine which point by: ndx mod(4)
				{
					case 0: // do something with uv-point A
							uvw.a=vuv;
						break;

					case 1: // do something with uv-point B
							uvw.b=vuv;
						break;

					case 2: // do something with uv-point C
							uvw.c=vuv;
						break;

					case 3: // do something with uv-point D[]
							uvw.d=vuv;
							break;
				}
				UVWTag::Set(dataptr, polyNdx, uvw);
			}
		}
	}

	return true;
}


Bool SnapAlign ::VReset(BaseContainer& data)
{
	data.SetFloat( MDATA_SNAP_VALUEX,0);
	data.SetFloat( MDATA_SNAP_VALUEY,0);
	data.SetFloat( MDATA_SNAP_VALUEZ,0);
	return true;
}

//3pointplane
Bool SnapAlign::Set3pointPlane(BaseDocument* doc, BaseContainer &data )
{
	ResetTool();
	EnableSnap(true, doc);
	EnableSnap(true, doc, SNAPMODE_POLYGON);
	EnableSnap(true, doc, SNAPMODE_POINT);

	data.SetBool(SNAP_ENABLEG , true);
	data.SetBool(SNAP_POLYGONG , true);
	data.SetBool(SNAP_POINTG , true);
	PlaneSet_BTNFlag=true;
	EventAdd();
	return true;
}

Bool SnapAlign::CreatePopupMenu(BaseContainer& menu,BaseContainer &data )
{
	if(data.GetBool(MDATA_SNAP_AX)) menu.SetString(ID_XMARK, "X&c&");
	else menu.SetString(ID_XMARK, "X");
	if(data.GetBool(MDATA_SNAP_AY)) menu.SetString(ID_YMARK, "Y&c&");
	else menu.SetString(ID_YMARK, "Y");
	if(data.GetBool(MDATA_SNAP_AZ)) menu.SetString(ID_ZMARK, "Z&c&");
	else menu.SetString(ID_ZMARK, "Z");
	return true;
}


Bool SnapAlign::SetVectorLen( BaseDocument* doc, BaseContainer& data, Vector P1, Vector P2)
{
	
	BaseObject* plane= GetWorkplaneObject( doc);
	EndMg=plane->GetMg();
//	P1.ToLV();
	P1=~EndMg*P1;
	P2=~EndMg*P2;
	data.SetVector(MDATA_PVECTOR,P2-P1);
	data.SetFloat(MDATA_LENGTHE,Len(P2-P1));
	return true;
}

void SnapAlign::TEST( BaseObject* obj)
{

	if(!obj) return;
	//PolygonObject* mesh = ToPoly(obj);
	//BaseSelect* bs = mesh->GetPolygonS();
	//Int32 ndx, seg = 0, smin, smax;
	//Int32 polyCount = mesh->GetPolygonCount();
	//AutoAlloc<Modeling> model;

	//NgonBase *ngons = ToPoly(obj)->GetNgonBase();
	//Ngon ngon;

	//Int32 i, pcnt = 0, ncnt = ToPoly(obj)->GetNgonCount(), vcnt = ToPoly(obj)->GetPolygonCount();

	//if (!model->InitObject(obj)) return;

	//// loop through all polygons

	//for (i = 0; i<vcnt; i++)
	//{
	//	if (!ngons || ngons->FindPolygon(i) == NOTOK) pcnt++;

	//	// read polygon as normal
	//}

	//while (bs->GetRange(seg++, polyCount, &smin, &smax))
	//{
	//	for (ndx = smin; ndx <= smax; ndx++)
	//	{
	//		if (!model->GetNgon(obj, ndx, &ngon)) return;
	//		Int32 * a = ngon.points;

	//		for (Int32 j = 0; j < ngon.count; j++)
	//		{
	//			GePrint("a:" + String::IntToString(a[j]));
	//		}
	//	}
	//}


	//GePrint("pcnt:" + String::IntToString(pcnt));
	//// loop through all ngons
	//GePrint("ncnt:" + String::IntToString(ncnt));
	//for (i = 0; i<ncnt; i++)
	//{
	//	GePrint("vcnt + i:" + String::IntToString(vcnt + i));
	//	if (!model->GetNgon(obj,  i, &ngon)) return ;
	//	Int32 * a = ngon.points;

	//	for (Int32 j = 0; j < ngon.count; j++)
	//	{
	//		GePrint("a:" + String::IntToString(a[j]));
	//	}
	//	// read points/segments from ngon
	//}

	//pcnt += ncnt;

	//GePrint("Count:" + String::IntToString(pcnt));





	PolygonObject* mesh=ToPoly(obj);
	// given mesh is a PolygonObject
//	const CPolygon* currentPoly = NULL;
	const CPolygon* polygons = NULL;
	const Vector* vertices = NULL;
 
	Int32 polyCount = mesh->GetPolygonCount();


	Int32 ngonIdx = NOTOK;
	Pgon* ngonData = NULL;


	Int32	 lNgonCount, *polymap = nullptr;
	DeleteMem(polymap);
//	Int32 **ngons = NULL;



	NgonBase* ngBase = mesh->GetNgonBase();
	if (ngBase)
	{

		ngonData = mesh->GetAndBuildNgon();

		//			Int32 polycnt = mesh->GetPolygonCount();
		if (!mesh->GetPolygonTranslationMap(lNgonCount, polymap))
			//if (!mesh->GetNGonTranslationMap(lNgonCount, polymap, ngons))
		{
			DeleteMem(polymap);
			return;
		}
		//if (!mesh->GetNGonTranslationMap(lNgonCount, polymap, ngons))
		//{
		//	DeleteMem(polymap);
		//	DeleteMem(ngons);
		//	return;
		//}
	}


	//Int32** nmap;

	//AutoAlloc<Modeling> mod;
	//if (!mod || !mod->InitObject(_Starget1)) return;
	//Int32 a;
//	mod->GetNgonMap(obj, nmap, &a);
	

	//GePrint("a=" + String::IntToString(a));

	//GePrint("nmap=" + String::IntToString(nmap[0][0]));





	polygons = mesh->GetPolygonR();
	vertices = mesh->GetPointR();
 
	BaseSelect* bs = mesh->GetPolygonS();

	Int32 ndx, seg = 0, smin, smax;

	//GePrint("selectcnt = " + String::IntToString(bs->GetCount()));
//	Int32 ngoncnt =mesh->GetNgonCount();
	while (bs->GetRange(seg++, polyCount, &smin, &smax))
	{
		for (ndx = smin; ndx <= smax; ndx++)
		{
			ngonIdx = NOTOK;
			if (ngBase)
			{
				ngonIdx = ngBase->FindPolygon(ndx);
				// GePrint( "ngonIdx="+String::IntToString(ngonIdx));
			}
			// if ngonIdx != NOTOK poly is a part of ngonIdx Ngon
			if (ngonIdx == NOTOK)
			{
				// Standard polygon
				GePrint("id = " + String::IntToString(ndx));
			}
			else
			{
				GePrint("ngon i=" + String::IntToString(ndx));
				//Int32 t;
				//Int32 *n = mod->GetPointNgons(obj, ndx, t, true);
				//GePrint("n=" + String::IntToString(n[0]));
				//Int32 x;
				//for (x = 0; x < ngoncnt; x++)
				//{
				//	for (Int32 y = 1; y <= ngons[x][0]; y++)
				//	{
				//		if (ngons[x][y] == ngonIdx)
				//		{
				//			Int32 a = ngons[x][y];
				//			GePrint("a=" + String::IntToString(a));
				//		}
				//	}

				//}
				//GePrint("ngon i=" + String::IntToString(ndx));


				//GePrint("lNgonCount = " + String::IntToString(lNgonCount));
				//for (Int32 j = 0; j < lNgonCount; j++)
				//{

				//	GePrint("aaa=" + String::IntToString(polymap[j]));
				//}
			}
		}
	}
//	if (!mod->Commit(obj, MODELING_COMMIT_UPDATE)) return;
//
	//--------------------------------------------------------------------------

	//for (Int32 i = 0; i < polyCount; ++i, ++currentPoly)
	//{
	//	// if ngBase there are some ngons in the mesh
	//	ngonIdx = NOTOK;
 //  
	//	if (ngBase)
	//	{
	//		ngonIdx = ngBase->FindPolygon(i);
	//	// GePrint( "ngonIdx="+String::IntToString(ngonIdx));
	//	}
	//	// if ngonIdx != NOTOK poly is a part of ngonIdx Ngon
	//	if (ngonIdx == NOTOK)
	//	{
	//	// Standard polygon

	//	}
	//	else
	//	{
	//		GePrint("ngon i=" + String::IntToString(i));
	//		Int32	 lNgonCount, *polymap = nullptr;
	//		DeleteMem(polymap);
	//		//			Int32 polycnt = mesh->GetPolygonCount();
	//		if (!mesh->GetPolygonTranslationMap(lNgonCount, polymap))
	//		{
	//			DeleteMem(polymap);
	//			return;
	//		}

	//		//	GetNGonTranslationMap
	//		GePrint("lNgonCount = " + String::IntToString(lNgonCount));
	//		for (Int32 j = 0; j < lNgonCount; j++)
	//		{

	//			GePrint("aaa=" + String::IntToString(polymap[j]));
	//		}
	//	}
	//}
}


//**********************************
// num = コピー元になるポリゴンID
// Cnt = コピー前のポリゴン数
// k =   コピーで増えた分のカウント数
//PoCnt = コピーしたポリゴン数
//map = 複製されたポリゴン数分のマップ

Int32 SnapAlign::CheckNgonID(BaseObject* op, Int32 num, Int32 Cnt, Int32 k, Char* map, Vector v, Int32 PoCnt, Int32 selcnt)
//Int32 SnapAlign::CheckNgonID(BaseObject* op, Int32 num,  Int32 Cnt, Int32 k,  Vector v, Int32 PoCnt)
{

//	GePrint("-------------------------");
 //  	DTOOL DP;
//	DP.printcheck("befor v=",v);
	//GePrint(" PoCn" + String::IntToString(PoCnt));
	if(!op) return -1;
	PolygonObject* mesh=ToPoly(op);
	if(!mesh) return -1;
	// given mesh is a PolygonObject

	const CPolygon* polygons = NULL;
	const Vector* vertices = NULL;

	Matrix mg=op->GetMg();
	mg.off=Vector();


	v=~mg*v;

	polygons = mesh->GetPolygonR();
	vertices = mesh->GetPointR();
	v=v*k;

	Int32 id  = -1;

		//	GePrint("----------------");
		//増えた分ごとに処理
	
	//GePrint("PoCnt=" + String::IntToString(PoCnt));
	//GePrint("k=" + String::IntToString(k));

	//DP.printcheck("Check v=",v);

//	GePrint("-------------------------");
	Int32 ak=k-1;
	for(Int32 i=0; i< PoCnt; i++)
	{

		Int32 na= i+Cnt;//コピーされたポリゴンの番号
		
	
		if (!map[i + ak*selcnt])
		{
		//	GePrint("na="+String::IntToString(na));
				
			if(	HikakuShape(vertices, polygons, num, na, v))
			{
				id=	na;
			   //GePrint("id="+String::IntToString(id));
			   //GePrint(" OK    i + ak*selcnt="+String::IntToString(i + ak*selcnt));
				map[i + ak*selcnt] = true;
				return id;
			}
		}
	//	map[i + k*selcnt] = true;

	}

	return id;
}

Bool SnapAlign::HikakuShape(const Vector* vv, const CPolygon* polyP,  Int32 Aa, Int32 Bb, Vector v)
{
//	GePrint("Aa="+String::IntToString(Aa));
 //  GePrint("Bb="+String::IntToString(Bb));
 
	Int32 na;
	if( polyP[Aa].c ==polyP[Aa].d ) na=3;
	else na=4;

	Int32 nb;
 	if( polyP[Bb].c ==polyP[Bb].d ) nb=3;
	else nb=4;

	if(na != nb) return false;

	Vector* v1 = NewMemClear(Vector, na);
	if(!v1) return false;

	v1[0]= vv[ polyP[Aa].a];
	v1[1]= vv[ polyP[Aa].b];
	v1[2]= vv[ polyP[Aa].c];
	if(na==4) v1[3]= vv[ polyP[Aa].d];
		
	Vector* v2=NewMemClear(Vector, nb);
	if(!v2)
	{
		DeleteMem(v1);
		return false;
	}

	v2[0]= vv[ polyP[Bb].a];
	v2[1]= vv[ polyP[Bb].b];
	v2[2]= vv[ polyP[Bb].c];
	if(nb==4) v2[3]= vv[ polyP[Bb].d];


	// DTOOL DP;
	//DP.printcheck("v=",v);

	Vector wa;
	Vector wb;
	for(Int32 i=0; i< na; i++)
	{
		 wa = wa+v1[i];
		 wb = wb+v2[i];
	}
	//  DP.printcheck("v =",v);
	wa/=na;
	wb/=na;
//	 DP.printcheck("wa/na=",wa);
//	  DP.printcheck("wb/nb=",wb);
	wa+=v;
//	DP.printcheck("wa+=v =",wa);
//	GePrint("  Len(wa)- Len(wb) > 0.0="+String::FloatToString(Abs(Len(wa)- Len(wb))));
	if(Abs(Len(wa- wb)) > 0.01)
	{
		DeleteMem(v1);
		DeleteMem(v2);
		return false;
	}


	DeleteMem(v1);
	DeleteMem(v2);

//	GePrint("Bb="+String::IntToString(Bb));
//	GePrint("true");
	return true;
}

////////////////////////////////////////////////////////////////////////////
Bool SnapTranceToolDialog::CreateLayout(void)
{

	SetTitle("Option");

	GroupBegin(0,BFH_LEFT,1,0,"",0);
	GroupBorderSpace(5, 5, 5, 5);
		AddCheckbox( MDATA_BOOL_LINEALIGN, BFH_SCALEFIT,0,10,"Linear line");
		GroupBegin(0,BFH_SCALEFIT,2,0,"",0);
			AddStaticText(0,BFH_LEFT,60, 10, "Count:", BORDER_NONE);
			AddEditNumberArrows(MDATA_BOOL_COPYCNT, BFH_SCALEFIT,30,0);
		GroupEnd();
		AddCheckbox( MDATA_COPYINS, BFH_SCALEFIT,0,10,"Instance");
		AddCheckbox(MDATA_RENDERINS, BFH_SCALEFIT, 0, 10, "Render Instance");
		AddSeparatorH(10,  BFH_FIT);
		AddStaticText(0, BFH_CENTER | BFV_TOP, 0, 0, "Snap target:", BORDER_NONE);
		AddComboBox(MDATA_ACTIONLIST, BFH_CENTER | BFV_TOP,120,0);
		AddChild(MDATA_ACTIONLIST, 0, "None");
		AddChild(MDATA_ACTIONLIST, 1, "Cpoy & Paste");
		AddChild(MDATA_ACTIONLIST, 2, "Cut & Paste");
		AddSeparatorH(10, BFH_FIT);
		//AddButton(MDATA_SETAXIS,BFH_SCALEFIT,0,10,"Set 3pointPlane");
		//AddButton(MDATA_RESET,BFH_SCALEFIT,0,10,"Reset");
		//AddSeparatorH(10,  BFH_FIT);

		AddCheckbox( MDATA_CHILD, BFH_SCALEFIT,0,10,"Child");
		AddCheckbox( MDATA_OPTIMIZE, BFH_SCALEFIT,0,10,"Weld");
		AddCheckbox( MDATA_BOOL_IDE_UVFIX, BFH_SCALEFIT,0,10,"Fix UVW");
		AddCheckbox(MDATA_SELTAG, BFH_SCALEFIT, 0, 10, "UVW&&SelectionTag");
		AddSeparatorH(10,  BFH_FIT);

		AddCheckbox( MDATA_NORMALPOSITION , BFH_SCALEFIT,0,10,"Normal Position");

	GroupEnd();
	return true;
}

Bool SnapTranceToolDialog::InitValues(void)
{

	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;
	SetBool(MDATA_BOOL_LINEALIGN, bc->GetBool(MDATA_BOOL_LINEALIGN));

	
	SetInt32(MDATA_BOOL_COPYCNT, bc->GetInt32(MDATA_BOOL_COPYCNT));
	SetBool(MDATA_COPYINS, bc->GetBool(MDATA_COPYINS));
	SetBool(MDATA_RENDERINS, bc->GetBool(MDATA_RENDERINS));
	SetBool(MDATA_CHILD, bc->GetBool(MDATA_CHILD));
	SetBool(MDATA_OPTIMIZE, bc->GetBool(MDATA_OPTIMIZE));
	SetBool(MDATA_BOOL_IDE_UVFIX, bc->GetBool(MDATA_BOOL_IDE_UVFIX));
	SetBool( MDATA_NORMALPOSITION, bc->GetBool(MDATA_NORMALPOSITION));
	SetBool(MDATA_SELTAG, bc->GetBool(MDATA_SELTAG));
	if (bc->GetInt32(MDATA_ACTIONLIST) == CH_NONE)SetInt32(MDATA_ACTIONLIST, 0);
	else if (bc->GetInt32(MDATA_ACTIONLIST) == CH_CUTPASTE)SetInt32(MDATA_ACTIONLIST, 1);
	else SetInt32(MDATA_ACTIONLIST, 2);
	EnableGadgets();

	return InitDialog();
}

Bool SnapTranceToolDialog::InitDialog(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;

	return true;
}

Bool SnapTranceToolDialog::Command(Int32 id,const BaseContainer &msg)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;
	switch (id)
	{
	case MDATA_BOOL_LINEALIGN:
	{
		Bool flag;
		GetBool(MDATA_BOOL_LINEALIGN, flag);
		bc->SetBool(MDATA_BOOL_LINEALIGN, flag);
	}
		break;
	case MDATA_COPYINS:
	{
		Bool flag;
		GetBool(MDATA_COPYINS, flag);
		bc->SetBool(MDATA_COPYINS, flag);
	}
		break;
	case MDATA_RENDERINS:
	{
		Bool flag;
		GetBool(MDATA_RENDERINS, flag);
		bc->SetBool(MDATA_RENDERINS, flag);
	}
		break;
	case MDATA_CHILD:
	{
		Bool flag;
		GetBool(MDATA_CHILD, flag);
		bc->SetBool(MDATA_CHILD, flag);
	}
		break;
	case MDATA_OPTIMIZE:
	{
		Bool flag;
		GetBool(MDATA_OPTIMIZE, flag);
		bc->SetBool(MDATA_OPTIMIZE, flag);
	}
		break;
	case MDATA_BOOL_IDE_UVFIX:
	{
		Bool flag;
		GetBool(MDATA_BOOL_IDE_UVFIX, flag);
		bc->SetBool(MDATA_BOOL_IDE_UVFIX, flag);
	}
		break;
	case MDATA_BOOL_COPYCNT:
	{

		Int32 Num;
		GetInt32(MDATA_BOOL_COPYCNT, Num);
		if (Num < 1)
		{
			Num = 1;
			SetInt32(MDATA_BOOL_COPYCNT, Num);

		}
		bc->SetBool(MDATA_BOOL_COPYCNT, Num);
	}
		break;
	case MDATA_NORMALPOSITION:
	{
		Bool flag;
		GetBool(MDATA_NORMALPOSITION, flag);
		bc->SetBool(MDATA_NORMALPOSITION, flag);
	}
		break;
	case MDATA_SELTAG:
	{
		Bool flag;
		GetBool(MDATA_SELTAG, flag);
		bc->SetBool(MDATA_SELTAG, flag);
	}
		break;
	case MDATA_ACTIONLIST:
	{
		Int32 a;
		GetInt32(MDATA_ACTIONLIST, a);
		if (a == 0)	bc->SetInt32(MDATA_ACTIONLIST, CH_NONE);
		else if (a == 1)	bc->SetInt32(MDATA_ACTIONLIST, CH_CUTPASTE);
		else bc->SetInt32(MDATA_ACTIONLIST, CH_COPYPASTE);
	
	}
		break;
	}
	return true;
}
void SnapTranceToolDialog::DestroyWindow()
{
	//pQuickTab=NULL;
}

Bool SnapTranceToolDialog::AskClose()
{

 //openFlag=false;
  return false;
}

Bool SnapTranceToolDialog::EnableGadgets(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;
	BaseDocument* doc=GetActiveDocument();
	if(!doc) return false;

	
	if( bc->GetInt32(MDATA_SNAP_MODE)==MDATA_CH_ALIGN) Enable(MDATA_CH_ALIGN,true);
	else Enable(MDATA_CH_ALIGN,false);

	if(doc->GetMode() == Mmodel || doc->GetMode() == Mobject )
	{
		Enable(MDATA_COPYINS,true);
		Enable(MDATA_CHILD,true);
		Enable(MDATA_NORMALPOSITION,true);
	}
	else 
	{
		Enable(MDATA_COPYINS,false);
		Enable(MDATA_CHILD,false);
		Enable(MDATA_NORMALPOSITION,false);
	}

	if (bc->GetInt32(MDATA_SNAP_MODE) == MDATA_CH_POSFIX && doc->GetMode() == Mpolygons)
	{
		Enable(MDATA_ACTIONLIST, true);
	}
	else
	{
		Enable(MDATA_ACTIONLIST, false);
	}
	if(bc->GetInt32(MDATA_SNAP_MODE)==MDATA_CH_POSFIX && (doc->GetMode() ==Mpolygons || doc->GetMode() == Mmodel || doc->GetMode() == Mobject) ) Enable(MDATA_BOOL_COPYCNT,true);
	else if(bc->GetInt32(MDATA_SNAP_MODE)==MDATA_CH_ALIGN && (doc->GetMode() ==Mpolygons || doc->GetMode() == Mmodel || doc->GetMode() == Mobject)) Enable(MDATA_BOOL_COPYCNT,true);
	else Enable(MDATA_BOOL_COPYCNT,false);

	return true;
}



////////////////////////////////////////////////////////////////////////////
//*****************************************************
//スナップダイアログ
//*****************************************************
Bool SnapToolDialog::CreateLayout(void)
{
	SetTitle("Snap Setting");

	GroupBegin(0,BFH_LEFT,1,0,"",0);
	GroupBorderSpace(5, 5, 5, 5);
		//GroupBegin(0,BFH_LEFT,2,0,"",0);
		//	GroupSpace(4,1);
		AddCheckbox( SNAP_ENABLEG, BFH_SCALEFIT,0,10,"Enable Snapping");
		AddSeparatorH(10,  BFH_FIT);
			AddComboBox(SNAP_METHODG,BFH_SCALEFIT,0,0);
			    AddChild(SNAP_METHODG, SNAP_AUTO, "Snap Auto");
		    	AddChild(SNAP_METHODG, SNAP_3DG, "Snap 3D");
		    	AddChild(SNAP_METHODG, SNAP_2DG, "Snap 2D");

			GroupBegin(0,BFH_SCALEFIT,2,0,"",0);
			    AddStaticText(0,BFH_LEFT,60, 10, "Radius:", BORDER_NONE);
                AddEditNumberArrows(SNAP_RADIUSG, BFH_SCALEFIT,30,0);
				AddCheckbox( MDATA_VISIBLE, BFH_SCALEFIT,0,10,"Visible Only");
		    GroupEnd();
            AddCheckbox( SNAP_PRE, BFH_SCALEFIT,0,10,"Preview");
		AddSeparatorH(10,  BFH_FIT);
			AddCheckbox( SNAP_QUANTIZUNG, BFH_SCALEFIT,0,10,"Enable Quantizing");
		AddSeparatorH(10,  BFH_FIT);

			AddCheckbox( SNAP_POINTG, BFH_SCALEFIT,0,10,"Vertex");
			AddCheckbox( SNAP_EDGEG, BFH_SCALEFIT,0,10,"Edge");
			AddCheckbox( SNAP_POLYGONG, BFH_SCALEFIT,0,10,"Polygon");
			AddCheckbox( SNAP_SPLINEG , BFH_SCALEFIT,0,10,"Spline");
			AddCheckbox( SNAP_AXISG  , BFH_SCALEFIT,0,10,"Axis");
			AddCheckbox( SNAP_INTERSECTIONG  , BFH_SCALEFIT,0,10,"Intersection");
			AddCheckbox( SNAP_MIDPOINTG, BFH_SCALEFIT,0,10,"Midpoint");
            GroupBegin(0,BFH_SCALEFIT,2,0,"",0);
		    	AddStaticText(SNAP_SUVTEXT,BFH_LEFT,60, 10, "Subdiv:", BORDER_NONE);
                AddEditNumberArrows(SNAP_SECTION, BFH_SCALEFIT,30,0);
            GroupEnd();

		AddSeparatorH(10,  BFH_FIT);
			AddCheckbox( SNAP_WORKPLANEG, BFH_SCALEFIT,0,10,"Workplane");
			AddCheckbox( SNAP_GRIDG, BFH_SCALEFIT,0,10,"Grid Point");
			AddCheckbox( SNAP_GRIDLINEG, BFH_SCALEFIT,0,10,"Grid Line");
		AddSeparatorH(10,  BFH_FIT);

			AddCheckbox( SNAP_GUIDE_SNAPG, BFH_SCALEFIT,0,10,"Guide Snap");
			AddCheckbox( SNAP_DYNAMICGUIDE_SNAPG, BFH_SCALEFIT,0,10,"Dynamic Guide");
			AddCheckbox( SNAP_PERPENDICULARG , BFH_SCALEFIT,0,10,"Prependicular Snap");
		
	GroupEnd();
	return true;
}


Bool SnapToolDialog::InitValues(void)
{

	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;
	BaseDocument* doc=GetActiveDocument();
	BaseContainer bcc = SnapSettings(doc);
		//Float x=bc->GetFloat(MDATA_SNAP_VALUEX);
	SetBool( SNAP_ENABLEG, bcc.GetBool(SNAP_SETTINGS_ENABLED));
	
	//SetBool(SNAP_ENABLEG, false);



	if(bcc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_AUTO)
	{
	//	data.SetInt32(SNAP_METHODG, SNAP_AUTO);
		SetInt32(SNAP_METHODG, SNAP_AUTO);
	}
	else if(bcc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_2D)
	{
		SetInt32(SNAP_METHODG, SNAP_3DG);
	}
	else
	{
		SetInt32(SNAP_METHODG,  SNAP_2DG);
	}

	SetInt32(SNAP_RADIUSG, bcc.GetInt32(SNAP_SETTINGS_RADIUS));

	SetBool( SNAP_POINTG, IsSnapEnabled(doc,SNAPMODE_POINT	));
	SetBool( SNAP_EDGEG, IsSnapEnabled(doc,SNAPMODE_EDGE	));
	SetBool( SNAP_POLYGONG, IsSnapEnabled(doc,SNAPMODE_POLYGON));
	SetBool( SNAP_MIDPOINTG, IsSnapEnabled(doc,SNAPMODE_MIDPOINT));
	SetBool( SNAP_GRIDG, IsSnapEnabled(doc,SNAPMODE_GRIDPOINT	));
	SetBool( SNAP_GRIDLINEG, IsSnapEnabled(doc,SNAPMODE_GRIDLINE	));
	SetBool( SNAP_SPLINEG, IsSnapEnabled(doc,SNAPMODE_SPLINE	));
	SetBool( SNAP_AXISG, IsSnapEnabled(doc, SNAPMODE_AXIS	));
	SetBool( SNAP_PERPENDICULARG, IsSnapEnabled(doc,SNAPMODE_ORTHO));

	SetBool( SNAP_QUANTIZUNG, IsSnapEnabled(doc,QUANTIZE_ENABLED ));
	SetBool( SNAP_INTERSECTIONG , IsSnapEnabled(doc, SNAPMODE_INTERSECTION	 ));
	SetBool( SNAP_WORKPLANEG , IsSnapEnabled(doc,SNAPMODE_WORKPLANE	));
	SetBool( SNAP_GUIDE_SNAPG , IsSnapEnabled(doc,SNAPMODE_GUIDE));
	//SetBool( SNAP_DYNAMICGUIDE_SNAPG , bc->GetBool( SNAP_DYNAMICGUIDE_SNAPG ));
	SetBool( SNAP_DYNAMICGUIDE_SNAPG , IsSnapEnabled(doc, SNAPMODE_DYNAMICGUIDE));

    SetInt32(SNAP_SECTION, bc->GetInt32(SNAP_SECTION));
    SetBool( SNAP_PRE , bc->GetBool(SNAP_PRE));
	SetBool( MDATA_VISIBLE , bc->GetBool(MDATA_VISIBLE));
 
	EnableGadgets();
	//openFlag=true;
	return InitDialog();
}

Bool SnapToolDialog::InitDialog(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;

	return true;
}

Bool SnapToolDialog::Command(Int32 id,const BaseContainer &msg)
{
	BaseDocument* doc=GetActiveDocument();
	BaseContainer bb = SnapSettings(doc);
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	//DTOOL DP;
	//SnapAlign sa;
	if (!bc) return false;
	switch (id)
	{
	case SNAP_ENABLEG:
		{

			Bool flag;
			GetBool(SNAP_ENABLEG, flag);
			bc->SetBool(SNAP_ENABLEG, flag);
			bb.SetBool(SNAP_SETTINGS_ENABLED,flag);
		}
		break;
	case SNAP_QUANTIZUNG:
		{

			Bool flag;
			GetBool(SNAP_QUANTIZUNG, flag);
			bc->SetBool(SNAP_QUANTIZUNG, flag);
			bb.SetBool(QUANTIZE_ENABLED,flag);
		}
		break;
	case SNAP_METHODG:
		{
			Int32 type;
			GetInt32(SNAP_METHODG, type);
			bc->SetInt32(SNAP_METHODG, type);
			if(type==SNAP_2DG) bb.SetInt32(SNAP_SETTINGS_MODE,SNAP_SETTINGS_MODE_2D);
			else if(type==SNAP_3DG) bb.SetInt32(SNAP_SETTINGS_MODE,SNAP_SETTINGS_MODE_3D);
			else bb.SetInt32(SNAP_SETTINGS_MODE, SNAP_SETTINGS_MODE_AUTO);
		}
		break;
	case SNAP_RADIUSG:
		{
			Int32 Num;
			GetInt32(SNAP_RADIUSG, Num);
			if(Num<1) Num = 1;
			SetInt32(SNAP_RADIUSG, Num);
			bc->SetInt32(SNAP_RADIUSG, Num);
		//	bb.SetFloat(SNAP_SETTINGS_RADIUS, (Float)Num);
		}
		break;
	case SNAP_POINTG:
		{

			Bool flag;
			GetBool(SNAP_POINTG, flag);
			bc->SetBool(SNAP_POINTG, flag);
			EnableSnap(flag,doc, SNAPMODE_POINT);
		}
		break;
	case SNAP_EDGEG:
		{

			Bool flag;
			GetBool(SNAP_EDGEG, flag);
			bc->SetBool(SNAP_EDGEG, flag);
			EnableSnap(flag, doc,  SNAPMODE_EDGE );
		}
		break;
	case SNAP_POLYGONG:
		{

			Bool flag;
			GetBool(SNAP_POLYGONG, flag);
			bc->SetBool(SNAP_POLYGONG, flag);
			EnableSnap(flag,doc,  SNAPMODE_POLYGON );
		}
		break;
	case SNAP_MIDPOINTG:
		{

			Bool flag;
			GetBool(SNAP_MIDPOINTG, flag);
			bc->SetBool(SNAP_MIDPOINTG, flag);
			EnableSnap(flag,doc,  SNAPMODE_MIDPOINT );
		}
		break;
	case SNAP_GRIDG:
		{

			Bool flag;
			GetBool(SNAP_GRIDG, flag);
			bc->SetBool(SNAP_GRIDG, flag);
			EnableSnap(flag,doc,   SNAPMODE_GRIDPOINT);
		}
		break;
	case SNAP_GRIDLINEG:
		{

			Bool flag;
			GetBool(SNAP_GRIDLINEG, flag);
			bc->SetBool(SNAP_GRIDLINEG, flag);
			EnableSnap(flag,doc,  SNAPMODE_GRIDLINE);
		}
		break;
	case SNAP_SPLINEG:
		{

			Bool flag;
			GetBool(SNAP_SPLINEG, flag);
			bc->SetBool(SNAP_SPLINEG, flag);
			EnableSnap(flag,doc,  SNAPMODE_SPLINE);
		}
		break;
	case SNAP_AXISG:
		{

			Bool flag;
			GetBool(SNAP_AXISG, flag);
			bc->SetBool(SNAP_AXISG, flag);
			EnableSnap(flag,doc,  SNAPMODE_AXIS); 
		}
		break;
	case SNAP_PERPENDICULARG:
		{

			Bool flag;
			GetBool(SNAP_PERPENDICULARG, flag);
			bc->SetBool(SNAP_PERPENDICULARG, flag);
			EnableSnap(flag,doc, SNAPMODE_ORTHO	);  
		}
		break;
	case SNAP_INTERSECTIONG:
		{

			Bool flag;
			GetBool(SNAP_INTERSECTIONG, flag);
			bc->SetBool(SNAP_INTERSECTIONG, flag);
			EnableSnap(flag,doc, SNAPMODE_INTERSECTION );  
		}
		break;
	case SNAP_WORKPLANEG:
		{

			Bool flag;
			GetBool(SNAP_WORKPLANEG, flag);
			bc->SetBool(SNAP_WORKPLANEG, flag);
			EnableSnap(flag,doc, SNAPMODE_WORKPLANE	);  
		}
		break;

	case SNAP_GUIDE_SNAPG:
		{

			Bool flag;
			GetBool(SNAP_GUIDE_SNAPG, flag);
			bc->SetBool(SNAP_GUIDE_SNAPG, flag);
			EnableSnap(flag,doc, SNAPMODE_GUIDE	);  
		}
		break;
	case SNAP_DYNAMICGUIDE_SNAPG:
		{

			Bool flag;
			GetBool(SNAP_DYNAMICGUIDE_SNAPG, flag);
			bc->SetBool(SNAP_DYNAMICGUIDE_SNAPG, flag);
			EnableSnap(flag,doc, SNAPMODE_DYNAMICGUIDE);  
		}
		break;
   case SNAP_SECTION:
		{
			Int32 Num;
			GetInt32(SNAP_SECTION, Num);
			if(Num<2) Num = 2;
			SetInt32(SNAP_SECTION, Num);
            bc->SetInt32(SNAP_SECTION, Num);
		}
		break;
        case SNAP_PRE:
        {
			Bool flag;
			GetBool(SNAP_PRE, flag);
			bc->SetBool(SNAP_PRE, flag);


        }
        break;
        case MDATA_VISIBLE:
        {
			Bool flag;
			GetBool(MDATA_VISIBLE, flag);
			bc->SetBool(MDATA_VISIBLE, flag);


        }
        break;

	}


	SnapSettings(doc, bb);
	//EventAdd();
	*checkFlag=true;
	EnableGadgets();
	return true;
}

void SnapToolDialog::EnableGadgets()
{

	Bool flag;
	GetBool( SNAP_ENABLEG, flag);

	Enable(SNAP_POINTG,flag);
	Enable( SNAP_EDGEG ,flag);
	Enable( SNAP_POLYGONG ,flag);
	Enable( SNAP_SPLINEG ,flag);
	Enable( SNAP_AXISG ,flag);
	Enable( SNAP_INTERSECTIONG ,flag);



	Enable( SNAP_WORKPLANEG ,flag);
	Bool flagB;
	GetBool(SNAP_WORKPLANEG, flagB);
	if(flag && flagB)
	{
		Enable(SNAP_GRIDG,true);
		Enable(SNAP_GRIDLINEG,true);
	}
	else
	{
		Enable(SNAP_GRIDG,false);
		Enable(SNAP_GRIDLINEG,false);
	}
	Enable( SNAP_GUIDE_SNAPG ,flag);
	GetBool(SNAP_GUIDE_SNAPG, flagB);
	if(flag && flagB)
	{
		Enable(SNAP_DYNAMICGUIDE_SNAPG,true);
		Enable( SNAP_PERPENDICULARG ,true);
	}
	else
	{
		Enable( SNAP_DYNAMICGUIDE_SNAPG,false);
		Enable( SNAP_PERPENDICULARG ,false);
	}


	Bool flagC, flagD;
	GetBool(  SNAP_EDGEG,  flagB);
	GetBool( SNAP_POLYGONG ,  flagC);
	GetBool( SNAP_SPLINEG ,  flagD);
	if(flag && (flagB || flagC || flagD))Enable( SNAP_MIDPOINTG ,true);
	else Enable( SNAP_MIDPOINTG ,false);
	
    Enable( SNAP_SECTION ,flag);
    Enable( SNAP_SUVTEXT ,flag);
}

void SnapToolDialog::DestroyWindow()
{
	//pQuickTab=NULL;
}

Bool SnapToolDialog::AskClose()
{

 //openFlag=false;
  return false;
}

void SnapToolDialog::SetFlag(Bool* flag)
{
	checkFlag=flag;
}





////////////////////////////////////////////////////////////////////////////
//*****************************************
//オフセットダイアログ
//*****************************************
Bool OffsetDialog::CreateLayout(void)
{

	GroupBegin(0,BFH_LEFT,0,1,"",0);
	GroupBorderSpace(5, 5, 5, 5);
		AddStaticText(MDATA_TAXUS,BFH_LEFT,15, 10, "", BORDER_NONE);
		AddEditNumberArrows(MDATA_OFFSET_TEXT,BFH_LEFT, 100, 10);
		AddButton(MDATA_REVERS ,BFH_SCALEFIT,0,5,"F");
		AddButton(MDATA_RESET,BFH_SCALEFIT,0,5,"R");
	GroupEnd();

	return true;
}

Bool OffsetDialog::InitValues(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;
	Float x=bc->GetFloat(MDATA_SNAP_VALUEX);
	Float y=bc->GetFloat(MDATA_SNAP_VALUEY);
	Float z=bc->GetFloat(MDATA_SNAP_VALUEZ);
	if(SetFlag==1)
	{
		SetString(MDATA_TAXUS, "X:");
		SetFloat(MDATA_OFFSET_TEXT,x);
	}
	else if(SetFlag==2)
	{
		SetString(MDATA_TAXUS, "Y:");
		SetFloat(MDATA_OFFSET_TEXT,y);
	}
	else if(SetFlag==3)
	{
		SetString(MDATA_TAXUS, "Z:");
		SetFloat(MDATA_OFFSET_TEXT,z);
	}
	//
	//SetFloat(MDATA_OffSetValueY,y);	
	//SetFloat(MDATA_OffSetValueZ,z);
	return InitDialog();
}

Bool OffsetDialog::InitDialog(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
	if (!bc) return false;

	return true;
}

Bool OffsetDialog::Command(Int32 id,const BaseContainer &msg)
{
	switch (id)
	{
		case MDATA_OFFSET_TEXT:
		{
			BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
			if (!bc) return false;
			Float value;
			GetFloat(MDATA_OFFSET_TEXT,value);
			if(SetFlag==1)
			{
				bc->SetFloat(MDATA_SNAP_VALUEX, value);
			}
			else if(SetFlag==2)
			{
				bc->SetFloat(MDATA_SNAP_VALUEY, value);
			}
			else if(SetFlag==3)
			{
				bc->SetFloat(MDATA_SNAP_VALUEZ, value);
			}

			EventAdd();
		}
		break;
		case MDATA_RESET:
		{
			SetInt32(MDATA_OFFSET_TEXT, 0);

			BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
			if (!bc) return false;
			if(SetFlag==1)
			{
				bc->SetFloat(MDATA_SNAP_VALUEX, 0);
			}
			else if(SetFlag==2)
			{
				bc->SetFloat(MDATA_SNAP_VALUEY, 0);
			}
			else if(SetFlag==3)
			{
				bc->SetFloat(MDATA_SNAP_VALUEZ, 0);
			}

			EventAdd();
		}
		break;
		case MDATA_REVERS:
			{
				BaseContainer *bc=GetToolData(GetActiveDocument(),ID_MODELING_SnapALIGN);
				if (!bc) return false;
				Float value;
				GetFloat(MDATA_OFFSET_TEXT,value);
				value*= -1;
				SetFloat(MDATA_OFFSET_TEXT,value);
				if(SetFlag==1)
				{
					bc->SetFloat(MDATA_SNAP_VALUEX, value);
				}
				else if(SetFlag==2)
				{
					bc->SetFloat(MDATA_SNAP_VALUEY, value);
				}
				else if(SetFlag==3)
				{
					bc->SetFloat(MDATA_SNAP_VALUEZ, value);
				}

				EventAdd();
			}
			break;
	}
	return true;
}
void OffsetDialog::DestroyWindow()
{
	//pQuickTab=NULL;
}

Bool OffsetDialog::AskClose()
{
	//GePrint("hhhh");

 //openFlag=false;
  return false;
}

Bool OffsetDialog::SetAdd( Int32 num)
{

	SetFlag=num;
  return true;
}
Bool OffsetDialog::RestoreLayout(void* secret)
{
	
	return true;
}


////////////////////////////////////////////////////////////////////////////
Bool SnapAlign::AddDrawViewPoint( BaseDocument* doc, BaseContainer& data, BaseDraw* bd,  BaseDrawHelp* bh )
{
		//DTOOL DP;
	AutoAlloc<AtomArray> arr;
	if (!arr) return false;
	doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	Int32 cnt=arr->GetCount();
	if(!cnt) return false;
	Int32 mode =doc->GetMode();
	Float sx= data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
	Float sy= data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
	Float sz= data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);
	Vector Vtp=Vector();

	Vector norm=Vector();

	Int32 cutCnt=data.GetInt32(SNAP_SECTION );
	for(Int32 i=0; i<cnt; i++)
	{
		BaseObject *op=(BaseObject *)arr->GetIndex(i);
		if(!op) return false;
		if( mode== Mpoints || mode == Medges || mode == Mpolygons )
		{
			
			if(op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd)
			{
				if(doc->GetMode() == Medges)
				{
					BaseContainer bc;
					ModelingCommandData cd;
					bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 1);
					bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
					cd.bc=&bc;
					cd.op = op;
					cd.mode=MODELINGCOMMANDMODE_ALL;
					if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
				}
				else if(doc->GetMode() == Mpolygons)
				{
					BaseContainer bc;
					ModelingCommandData cd;
					bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
					bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
					cd.bc=&bc;
					cd.op = op;
					cd.mode=MODELINGCOMMANDMODE_ALL;
					if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
				}

				PointObject *pp=ToPoint(op);
				const Vector *Cvp=pp->GetPointR();
				BaseSelect *bs=pp->GetPointS();

				Int32 SelCnt=bs->GetCount();
				if(SelCnt)
				{
					Int32 ndx, seg = 0, smin, smax;
					Matrix mg=op->GetMg();
					Vector tgv;

					if(Drag )//ドラッグ中の動作
					{
						bd->SetMatrix_Matrix(NULL, bh->GetMg());
						bd->DrawLine(Mp1,G_markpoint,0 );

                        SPANV[0]=G_markpoint;
				        SPANV[1]=Mp1;
				        Vector unit=(G_markpoint-Mp1)/cutCnt;
				        Vector fv=Mp1;
				        for(Int32 j=1; j< cutCnt; j++)
				        {
					        bd->DrawHandle(fv+unit*i, DRAWHANDLE_BIG, 0);
				        }
						bd->SetMatrix_Matrix(NULL, bh->GetMg());
				        bd->DrawHandle(Mp1, DRAWHANDLE_BIG, 0);

						//分割スナップが２の場合
				        if(cutCnt==2)
                        {
                             tgv=(G_markpoint+Mp1)/2;
                        }
                        else
                        {
							//分割数が２以外の場合
					        CutSnapFlag=1;
                            tgv=G_markpoint;
				        }
					}
					else 
					{
						if(AltKey) //Ctrlキー　で有効軸設定時一時移動中止
						{
							tgv=LocV;
						}
						else
						{
							tgv=G_markpoint;
							if (CutSnapFlag == 2) tgv = MMP;
						}
					}
					//ラインモード
					if(data.GetBool(MDATA_BOOL_LINEALIGN))
					{
						norm=tgv-G_P1;
						norm.Normalize();
					}
					Vtp=tgv;

					Matrix Smg=EndMg;
					Smg.off=Vector();

					while( bs->GetRange(seg++, pp->GetPointCount(), &smin,&smax) )
					{
						for( ndx=smin; ndx<=smax; ndx++ )
						{
							Vector Pre=Cvp[ndx]; //ローカル座標
							Pre=mg*Pre;	//グローバル座標
							if(G_SnapFlag)
							{
								Vector v;
								if(data.GetBool(MDATA_BOOL_LINEALIGN))
								{
									v=PointLineDistance(G_P1,norm,Pre);
									v*=-1;
								}
								else
								{
									v=tgv-Pre;
								}
								Vector vx=~Smg*v;
								Vector vc=Vector(0,0,0);
								if(data.GetBool(MDATA_SNAP_AX))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{

										vx.x=vx.x+sx;
									}
									vc=vc+Vector(vx.x,0,0);
								}
								if(data.GetBool(MDATA_SNAP_AY))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										
										vx.y=vx.y+sy;
									}
									vc=vc+Vector(0,vx.y,0);
								}
								if(data.GetBool(MDATA_SNAP_AZ))
								{
									if(data.GetBool(MDATA_BOOL_OFFSET))
									{
										
										vx.z=vx.z+sz;
									}
									vc=vc+Vector(0,0,vx.z);
								}
								bd->SetMatrix_Matrix(NULL, bh->GetMg());
								vc=Smg*vc;
								Vector get=Pre+vc;
								bd->DrawLine(Pre,get, 0);
								bd->DrawHandle(get, DRAWHANDLE_BIG, 0);
							}
							else
							{

								bd->SetMatrix_Matrix(NULL, bh->GetMg());
								bd->DrawLine(Pre,tgv, 0);
								bd->DrawHandle(tgv, DRAWHANDLE_BIG, 0);
							}
						}
					}
				}
			}
		}
		else if(mode == Mobject || mode == Mmodel )
		{
			Vector tgv;
			//ドラッグ時の描画設定
			if(Drag)
			{
				//１点目からマウスカーソルまでライン描画
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->DrawLine(Mp1,G_markpoint , 0);
				//printcheck("Mp1=", Mp1);
				//printcheck("G_markpoint=", G_markpoint);
				SPANV[0]=G_markpoint;
				SPANV[1]=Mp1;


				Vector unit=(G_markpoint-Mp1)/cutCnt;
				Vector fv=Mp1;
				for(Int32 k=1; k< cutCnt; k++)
				{
					bd->DrawHandle(fv+unit*k, DRAWHANDLE_BIG, 0);
				}

				bd->DrawHandle(Mp1, DRAWHANDLE_BIG, 0);
				if(cutCnt==2)
                {
                     tgv=(G_markpoint+Mp1)/2;
                }
                else
                {
					CutSnapFlag=1;
                    tgv=G_markpoint;
				}
				
			}
			else
			{
				if(AltKey)
				{
					tgv=LocV;
				}
				else
				{	
					tgv=G_markpoint;
					if (CutSnapFlag == 2) tgv = MMP;
				}
			}


			//ラインモードp
			if(data.GetBool(MDATA_BOOL_LINEALIGN))
			{
				norm=tgv-G_P1;
				norm.Normalize();
			}
			
            

			Vtp=tgv;
            //スナップオン
			if(G_SnapFlag != 0)
			{

				
				Matrix Dmg=op->GetMg();
				Vector ipos=Dmg.off;
				Vector pos;

				if(data.GetBool(MDATA_BOOL_LINEALIGN))
				{
					pos=PointLineDistance(G_P1,norm,ipos);
					pos*=-1;
				}
				else
				{
					pos=tgv-ipos;
				}

				Matrix Smg=EndMg;
				Smg.off=Vector();
				pos=~Smg*pos;
				Vector vc=Vector(0,0,0);
				if(data.GetBool(MDATA_SNAP_AX))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.x=pos.x+sx;
					}
					vc=vc+Vector(pos.x,0,0);
				}
				if(data.GetBool(MDATA_SNAP_AY))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.y=pos.y+sy;
					}
					vc=vc+Vector(0,pos.y,0);
				}
				if(data.GetBool(MDATA_SNAP_AZ))
				{
					if(data.GetBool(MDATA_BOOL_OFFSET))
					{
						pos.z=pos.z+sz;
					}
					vc=vc+Vector(0,0,pos.z);
				}
				vc=Smg*vc;
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				Vector get=vc+ipos;
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				if(cutCnt == 2|| CutSnapFlag==2)
				{
					
					bd->DrawHandle(get, DRAWHANDLE_BIG, 0);
				}
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				//マウスカーソルまでラインを描画
                bd->DrawLine(Dmg.off,get, 0);
				bd->DrawHandle(Dmg.off, DRAWHANDLE_BIG, 0);
	

		
				//bd->SetMatrix_Matrix(NULL, bh->GetMg());
						//bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
			//	if(data.GetBool( SNAP_PRE) && (cutCnt==2 || CutSnapFlag==2))
                if((data.GetBool( SNAP_PRE) && cutCnt==2) || (data.GetBool( SNAP_PRE) && CutSnapFlag==2)   )
				{
					Vector colar=data.GetVector(PRE_COLOR);
					bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_LOWEREQUAL);	
					ModelingCommandData mcd;
					mcd.doc = doc;
					mcd.op = op;
					if(!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, mcd)) return false;
					BaseObject*cp = static_cast<BaseObject*>(mcd.result->GetIndex(0));
					DTOOL DP;
					BaseObject*res=NULL;
	
					if(cp->GetDown())
					{
						AutoAlloc <AtomArray> arr2;
						arr2->Append(cp);
						DP.search_hierarchy( cp->GetDown(), cp, arr2);

						BaseContainer bc;
						bc.SetBool(MDATA_JOIN_MERGE_SELTAGS, true);

						ModelingCommandData mcd2;
						mcd2.doc = NULL;
						mcd2.arr=arr2;
						mcd2.bc=&bc;
						mcd2.mode = MODELINGCOMMANDMODE_ALL;
						if(!SendModelingCommand(MCOMMAND_JOIN, mcd2)) return NULL;
						res = static_cast<BaseObject*>(mcd2.result->GetIndex(0));
						AtomArray::Free(mcd2.result);
						Matrix rx;
						BaseObject::Free(cp);
						cp=NULL;
						cp=res;
						cp->SetMg(rx);
					}
					Matrix zmg;
					if(cp->GetType() == Opolygon)
					{
						Matrix am=op->GetMg();
						Vector scl=DP.GetScaleForMatrix(am);
						zmg=MatrixScale(scl);

						if(data.GetBool( MDATA_NORMALPOSITION))
						{
							zmg=Smg*zmg;
						}
						else
						{
							zmg=Smg*am;

						}
						
						zmg.off=get;
						cp->SetMg(zmg);

						if(bd->DrawPolygonObject(bh,cp, DRAWOBJECT_XRAY_ON | DRAWOBJECT_USE_CUSTOM_COLOR | DRAWOBJECT_NO_EOGL, NULL, colar) == DRAWRESULT_ERROR) return TOOLDRAW_0;
	
					}
					
				
					if(PShift)
					{
						Vector off=get-Dmg.off;
						//Vector pos=zmg.off;
						for(Int32 j=1; j<data.GetInt32(MDATA_BOOL_COPYCNT); j++)
						{
							AutoAlloc<AliasTrans> trans;
							BaseObject* Clone=(BaseObject*)cp->GetClone(COPYFLAGS_0,trans);
							if(Clone->GetType() == Opolygon)
							{
								zmg.off=pos+(off*j);
								Clone->SetMg(zmg);
								if(bd->DrawPolygonObject(bh,Clone, DRAWOBJECT_XRAY_ON | DRAWOBJECT_USE_CUSTOM_COLOR | DRAWOBJECT_NO_EOGL, NULL, colar) == DRAWRESULT_ERROR) return TOOLDRAW_0;
								
							}
							BaseObject::Free(Clone);
							Clone=NULL;
						}
					}
					BaseObject::Free(cp);
					cp=NULL;
				}
			}
			else
			{
				

				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				Matrix Dmg=op->GetMg();
				bd->DrawLine( Dmg.off,tgv, 0);
				bd->DrawHandle(Dmg.off, DRAWHANDLE_BIG, 0);
			}
		}
	}

	//分割スナップモード有効時分割ポイントを描画
	if( CutSnapFlag )
	{
        bd->SetMatrix_Matrix(NULL, bh->GetMg());
        bd->SetPen(Vector(255 ,255,255));
		bd->DrawHandle(SPANV[1], DRAWHANDLE_BIG, 0);
		bd->DrawHandle(SPANV[0], DRAWHANDLE_BIG, 0);
		bd->DrawLine(SPANV[1],SPANV[0] , 0);


		cutCnt=data.GetInt32(SNAP_SECTION );
		Vector unit=(SPANV[0]-SPANV[1])/cutCnt;
		Vector fv=SPANV[1];
		
        DTOOL DP;
		for(Int32 i=1; i< cutCnt; i++)
		{
            Vector pv=fv+unit*i;
			if(DP.FuzzyPointInRange( data,bd , pv, MpRX[0],MpRX[1]))
			{
                bd->SetPen(Vector(255 ,0,0));
			}
            else
            {
                bd->SetPen(Vector(0,0,255));
            }
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->DrawHandle(fv+unit*i, DRAWHANDLE_BIG, 0);
		}
	}

	//Linerモード有効時
	if( data.GetBool(MDATA_BOOL_LINEALIGN) )
	{
		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		bd->SetPen(Vector(255,255,0));
		bd->DrawLine(G_P1,Vtp, 0);
		bd->SetPen(Vector(255,0,0));
		bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
		bd->DrawHandle(Vtp, DRAWHANDLE_BIG, 0);
	}

	return true;
}


////*************************************************************
////ツールのスナップ設定からシステムのスナップ設定をあわせる
////*************************************************************
//Bool SnapAlign::SnapSync(BaseDocument* doc, BaseContainer& data)
//{
//	DTOOL DP;
//	DP.SetCHECKUP(doc, data, PlaneSet_BTNFlag);
//
//	if (!doc) return false;
//	EnableSnap(data.GetBool(SNAP_POINTG), doc, SNAPMODE_POINT);
//	EnableSnap(data.GetBool(SNAP_SPLINEG), doc, SNAPMODE_SPLINE);
//	EnableSnap(data.GetBool(SNAP_MIDPOINTG), doc, SNAPMODE_MIDPOINT);
//	EnableSnap(data.GetBool(SNAP_EDGEG), doc, SNAPMODE_EDGE);
//	EnableSnap(data.GetBool(SNAP_AXISG), doc, SNAPMODE_AXIS);
//	EnableSnap(data.GetBool(SNAP_POLYGONG), doc, SNAPMODE_POLYGON);
//	EnableSnap(data.GetBool(SNAP_INTERSECTIONG), doc, SNAPMODE_INTERSECTION);
//	EnableSnap(data.GetBool(SNAP_WORKPLANEG), doc, SNAPMODE_WORKPLANE);
//	EnableSnap(data.GetBool(SNAP_GRIDG), doc, SNAPMODE_GRIDPOINT);
//	EnableSnap(data.GetBool(SNAP_GRIDLINEG), doc, SNAPMODE_GRIDLINE);
//	EnableSnap(data.GetBool(SNAP_GUIDE_SNAPG), doc, SNAPMODE_GUIDE);
//	EnableSnap(data.GetBool(SNAP_DYNAMICGUIDE_SNAPG), doc, SNAPMODE_DYNAMICGUIDE);
//	EnableSnap(data.GetBool(SNAP_PERPENDICULARG), doc, SNAPMODE_ORTHO);
//
//
//	EventAdd();
//	return true;
//}


void SnapAlign::Priinit(BaseContainer& data, Bool flag )
{

	String str= "SnP.dat";
	Filename Fpath = GeGetC4DPath(C4D_PATH_PREFS);
	String path = Fpath.GetString();
	//一つ上のディレクトリを取得
	Int32 pos;
#ifdef __PC
	{
		path.FindLast("\\", &pos, -1);
	}
#else
	{
		path.FindLast("/", &pos, -1);
	}
#endif

	String lastWord = path.SubStr(0, pos);  //Grab the last word (sub string)

#ifdef __PC
	{
		lastWord += "\\plugins\\SnapPRS\\" + str;
	}
#else
	{
		lastWord += "/plugins/SnapPRS/" + str;
	}
#endif

	Fpath.SetString(lastWord);
	AutoAlloc<HyperFile> hf;

	if(flag )
	{
		if (hf->Open('temp', Fpath, FILEOPEN_WRITE, FILEDIALOG_IGNOREOPEN))
		{
			SaveData(hf,data);
			String s = Fpath.GetString();
			MessageDialog("Set to the default\n" + s);

		}
	}
	else
	{
		//ファイル読み込み
		if (hf->Open('temp', Fpath, FILEOPEN_READ, FILEDIALOG_IGNOREOPEN))
		{
			BaseContainer bc;
			hf->ReadContainer(&bc,true);
			String ver=GeLoadString(IDC_VER);
			String s=bc.GetString(999);
			if(ver !=  s || ver.GetLength() ==0)
			{

				DefoultSet( data);
				hf->Close();
				if (hf->Open('temp', Fpath, FILEOPEN_WRITE, FILEDIALOG_IGNOREOPEN))
				{
					SaveData(hf,data);
				}
			}
			else
			{

				ReadData( bc,  data);
			}
		}
		else
		{

			DefoultSet( data);
			hf->Close();
			if (hf->Open('temp', Fpath, FILEOPEN_WRITE, FILEDIALOG_IGNOREOPEN))
			{
				SaveData(hf,data);
			}
		}
	}

	hf->Close();
	hf.Free();
}



//パラメータ読み込み
void SnapAlign::ReadData(BaseContainer bc, BaseContainer& data)
{
	data.SetFloat(MDATA_SNAP_VALUEX, bc.GetFloat( MDATA_SNAP_VALUEX));
	data.SetFloat(MDATA_SNAP_VALUEY, bc.GetFloat( MDATA_SNAP_VALUEY));
	data.SetFloat(MDATA_SNAP_VALUEZ, bc.GetFloat( MDATA_SNAP_VALUEZ));
	data.SetInt32(MDATA_SNAP_MODE,bc.GetInt32(MDATA_SNAP_MODE) );//モードをAlignに設定
	data.SetBool(MDATA_SNAP_AX, bc.GetBool( MDATA_SNAP_AX));
	data.SetBool(MDATA_SNAP_AY, bc.GetBool( MDATA_SNAP_AY));
	data.SetBool(MDATA_SNAP_AZ, bc.GetBool( MDATA_SNAP_AZ));
	data.SetBool( MDATA_CHILD, bc.GetBool( MDATA_CHILD));
	data.SetInt32( MDATA_BOOL_COPYCNT, bc.GetInt32(MDATA_BOOL_COPYCNT) );
	data.SetVector(PRE_COLOR, bc.GetVector( PRE_COLOR));

	data.SetInt32( IDE_Snap_AXIS, bc.GetInt32( IDE_Snap_AXIS));
	data.SetBool(MDATA_BOOL_GRID_FLAG, bc.GetBool(MDATA_BOOL_GRID_FLAG) );
	data.SetFloat( MDATA_GRID_VALUE, bc.GetFloat(MDATA_GRID_VALUE)  );
	data.SetInt32( MDATA_GRIDLINE, bc.GetInt32( MDATA_GRIDLINE) );
	data.SetInt32( MDATA_LINE, bc.GetInt32(MDATA_LINE)  );
	data.SetInt32( MDATA_GRID_LIST, bc.GetInt32(MDATA_GRID_LIST) );
	data.SetInt32(SNAP_SECTION,bc.GetInt32(SNAP_SECTION));
	data.SetBool(MDATA_VISIBLE, bc.GetBool(MDATA_VISIBLE));
	data.SetInt32( MDATA_ACTIONLIST, bc.GetInt32(MDATA_ACTIONLIST));

	data.SetBool(MDATA_SELTAG, bc.GetBool(MDATA_SELTAG));
	data.SetBool(MDATA_RENDERINS, bc.GetBool(MDATA_RENDERINS));
}

//デフォルトデータセット
void SnapAlign::DefoultSet( BaseContainer& data)
{
	data.SetFloat(MDATA_SNAP_VALUEX, 0.0f);
	data.SetFloat(MDATA_SNAP_VALUEY, 0.0f);
	data.SetFloat(MDATA_SNAP_VALUEZ, 0.0f);
	data.SetInt32(MDATA_SNAP_MODE,MDATA_CH_ALIGN);//モードをAlignに設定
	data.SetBool(MDATA_SNAP_AX, true);
	data.SetBool(MDATA_SNAP_AY, true);
	data.SetBool(MDATA_SNAP_AZ, true);
	data.SetBool( MDATA_CHILD, true);
	data.SetInt32( MDATA_BOOL_COPYCNT, 1);
	data.SetVector(PRE_COLOR, Vector(0,0,1));
	data.SetInt32(SNAP_SECTION, 2);

	data.SetInt32( IDE_Snap_AXIS, IDE_XZ);
	data.SetBool(MDATA_BOOL_GRID_FLAG, false);
	data.SetFloat( MDATA_GRID_VALUE, 100 );
	data.SetInt32( MDATA_GRIDLINE, 100 );
	data.SetInt32( MDATA_LINE, 10 );
	data.SetInt32( MDATA_GRID_LIST, CH_GRID_L1);
	data.SetBool( MDATA_VISIBLE, false);
	data.SetInt32( MDATA_ACTIONLIST, CH_NONE);
	data.SetBool( MDATA_SELTAG, false);
	data.SetBool(MDATA_RENDERINS, false);

}


//パラメータ保存
void SnapAlign::SaveData(HyperFile* hf, BaseContainer& data)
{
	BaseContainer bc;

	bc.SetFloat(MDATA_SNAP_VALUEX, data.GetFloat( MDATA_SNAP_VALUEX));
	bc.SetFloat(MDATA_SNAP_VALUEY, data.GetFloat( MDATA_SNAP_VALUEY));
	bc.SetFloat(MDATA_SNAP_VALUEZ, data.GetFloat( MDATA_SNAP_VALUEZ));
	bc.SetInt32(MDATA_SNAP_MODE,data.GetInt32(MDATA_SNAP_MODE) );//モードをAlignに設定
	bc.SetBool(MDATA_SNAP_AX, data.GetBool( MDATA_SNAP_AX));
	bc.SetBool(MDATA_SNAP_AY, data.GetBool( MDATA_SNAP_AY));
	bc.SetBool(MDATA_SNAP_AZ, data.GetBool( MDATA_SNAP_AZ));
	bc.SetBool( MDATA_CHILD, data.GetBool( MDATA_CHILD));
	bc.SetInt32( MDATA_BOOL_COPYCNT, data.GetInt32(MDATA_BOOL_COPYCNT) );
	bc.SetVector(PRE_COLOR, data.GetVector( PRE_COLOR));

	bc.SetInt32( IDE_Snap_AXIS, data.GetInt32( IDE_Snap_AXIS));
	bc.SetBool(MDATA_BOOL_GRID_FLAG, data.GetBool(MDATA_BOOL_GRID_FLAG) );
	bc.SetFloat( MDATA_GRID_VALUE, data.GetFloat(MDATA_GRID_VALUE)  );
	bc.SetInt32( MDATA_GRIDLINE, data.GetInt32( MDATA_GRIDLINE) );
	bc.SetInt32( MDATA_LINE, data.GetInt32(MDATA_LINE)  );
	bc.SetInt32( MDATA_GRID_LIST, data.GetInt32(MDATA_GRID_LIST) );
	bc.SetInt32(SNAP_SECTION,data.GetInt32(SNAP_SECTION));
	bc.SetBool(MDATA_VISIBLE, data.GetBool( MDATA_VISIBLE));
	bc.SetInt32(MDATA_ACTIONLIST,data.GetInt32(MDATA_ACTIONLIST));
	bc.SetBool(MDATA_SELTAG, data.GetBool( MDATA_SELTAG));
	bc.SetBool(MDATA_RENDERINS, data.GetBool(MDATA_RENDERINS));
	String ver=GeLoadString(IDC_VER);
	bc.SetString( 999,ver);
	hf->WriteContainer(bc);
}



////////////////////////////////////////////////////////////////////////////
Bool SnapAlign::AddDrawEdgeExtension(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh)
{
	AutoAlloc<AtomArray> arr;
	if (!arr) return false;
	doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
	Int32 cnt = arr->GetCount();
	if (!cnt) return false;
//	Int32 mode = doc->GetMode();
	Float sx = data.GetFloat(MDATA_SNAP_VALUEX, 0.0f);
	Float sy = data.GetFloat(MDATA_SNAP_VALUEY, 0.0f);
	Float sz = data.GetFloat(MDATA_SNAP_VALUEZ, 0.0f);


	bd->SetMatrix_Matrix(NULL, bh->GetMg());
	bd->SetPen(Vector(1.0, 1.0, 0.0));
	bd->DrawLine(G_P1, G_markpoint, 0);
	bd->SetPen(Vector(1.0, 0.0, 0.0));
	bd->DrawHandle(G_P1, DRAWHANDLE_BIG, 0);
	
	Matrix Smg = EndMg;
	Smg.off = Vector();

	DTOOL DP;

	
//	Int32 cutCnt = data.GetInt32(SNAP_SECTION);
	for (Int32 i = 0; i < cnt; i++)
	{
		BaseObject *op = (BaseObject *)arr->GetIndex(i);
		if (!op) return false;


		if (op->GetType() == Opolygon)
		{

            PolygonObject* Poly = ToPoly(op);
            if (Poly)
            {
				PointObject* point = ToPoint(op);
				Vector* vp = point->GetPointW();
				Matrix pmg = op->GetMg();
                BaseSelect* Ebs = Poly->GetEdgeS();
                Int32 ecnt = Ebs->GetCount();
                const CPolygon*  poly = Poly->GetPolygonR(); //ポリゴンリスト取得
                if (ecnt)
                {
                    
                    Int32 ndx, seg = 0, smin, smax;
                    while (Ebs->GetRange(seg++, Poly->GetPolygonCount() * 4, &smin, &smax))
                    {
                        for (ndx = smin; ndx <= smax; ndx++)
                        {

							Int32 Id_e = ndx % 4; //エッジインデックスを取得
							Int32 polyNdx = (ndx - Id_e) / 4; //ポリゴンIDを取得

							Int32 a, b, c;
							a = NULL;
							b = NULL;
							c = NULL;


							//エッジにつながるポイントインデックス取得
							switch (Id_e)
							{
							case 0: a = poly[polyNdx].a; b = poly[polyNdx].b; c = poly[polyNdx].c; break;
							case 1: a = poly[polyNdx].b; b = poly[polyNdx].c; c = poly[polyNdx].d; break;
							case 2: a = poly[polyNdx].c; b = poly[polyNdx].d; c = poly[polyNdx].a; break;
							case 3: a = poly[polyNdx].d; b = poly[polyNdx].a; c = poly[polyNdx].b; break;
							}
							//GePrint("a=" + String::IntToString(a));
							//GePrint("b=" + String::IntToString(b));
							Vector va = pmg*vp[a];
							Vector vb = pmg*vp[b];

							//交点を求める
							Vector pv1, pv2;
							DP.CrossSectionB(G_P1, G_markpoint, va, vb, &pv1, &pv2, data, EndMg);

							//交点ポイントを描画
							bd->SetMatrix_Matrix(NULL, bh->GetMg());
							bd->SetPen(Vector(0.0, 1.0, 0.0));

							//交点に近いエッジポイントを取得
							Vector vt;

							if (Len(pv2 - va) < Len(pv2 - vb))
							{
								vt=va;		
							}
							else
							{
								vt=vb;
							}
							//エッジポイントと交点の差を取得
							Vector leg = pv2 - vt;
							//基準平面にローカル化
							Vector vx=~Smg*leg;
							Vector vc = Vector(0, 0, 0);
							if (data.GetBool(MDATA_SNAP_AX))
							{
								if (data.GetBool(MDATA_BOOL_OFFSET))
								{

									vx.x = vx.x + sx;
								}
								vc = vc + Vector(vx.x, 0, 0);
							}
							if (data.GetBool(MDATA_SNAP_AY))
							{
								if (data.GetBool(MDATA_BOOL_OFFSET))
								{

									vx.y = vx.y + sy;
								}
								vc = vc + Vector(0, vx.y, 0);
							}
							if (data.GetBool(MDATA_SNAP_AZ))
							{
								if (data.GetBool(MDATA_BOOL_OFFSET))
								{

									vx.z = vx.z + sz;
								}
								vc = vc + Vector(0, 0, vx.z);
							}
							vc = Smg*vc;
							pv2=vt+vc;
							bd->DrawLine(vt, pv2, 0);
							bd->SetPen(Vector(0.0, 0.0, 1.0));
							bd->DrawHandle(pv2, DRAWHANDLE_BIG, 0);

                        }
                    }
                
                }
            }
        }
    }
    return true;
}
                        
  
//********************************************************************************
//ポリゴンコピー
//********************************************************************************
Bool SnapAlign::ActionPolyMove(BaseDocument* doc, BaseContainer& data, Mobj Amobj, Vector vc)
{

  	DTOOL DP;
	
	PointObject* pp=ToPoint(Amobj.op);
	if(!pp) return false;
	PolygonObject* Poly=ToPoly(Amobj.op);
	Int32 PolyCnt=Poly->GetPolygonCount();

	BaseSelect* Pbs = Poly->GetPolygonS();

	UVWTag *m_uvTag=(UVWTag*)Amobj.op->GetTag(Tuvw);
	SelectionTag* seltag = (SelectionTag*) Amobj.op->GetTag(Tpolygonselection);
				

	//コピー数
	Int32 CCnt=data.GetInt32(MDATA_BOOL_COPYCNT);
	//ポリゴンクローン
	BaseContainer bc;
	bc.SetInt32(MDATA_CLONE_CNT,CCnt);
	bc.SetFloat(MDATA_CLONE_OFFSET,0);
	bc.SetFloat(MDATA_CLONE_HOLES, 0);
	bc.SetFloat(MDATA_CLONE_SCALE,1);
	bc.SetInt32(MDATA_CLONE_AXIS,0);
	bc.SetFloat(MDATA_CLONE_ROTATION,0);
	bc.SetVector(MDATA_CLONE_LOCALMOVE,Vector(0,0,0));
	bc.SetVector(MDATA_CLONE_LOCALSCALE,Vector(1,1,1));
	bc.SetVector(MDATA_CLONE_LOCALROTATE,Vector(0,0,0));
	bc.SetBool(MDATA_CLONE_KEEPFORM, true);

	Int32 Fcnt=pp->GetPointCount();
	ModelingCommandData gmcd;
	gmcd.doc = NULL;
	gmcd.op = Amobj.op;
	gmcd.bc = &bc;
	gmcd.mode =  MODELINGCOMMANDMODE_POLYGONSELECTION;
	gmcd.flags = MODELINGCOMMANDFLAGS_0;

	if (!SendModelingCommand(ID_MODELING_CLONE_TOOL, gmcd)) return false;
			
	Vector* vv=pp->GetPointW();
	if(!vv) return false;
					
	Int32 k=1;
					

	for(Int32 j=0; j< Amobj.scnt*CCnt; j++)
	{
		Vector off=vc;
		Vector p=vv[j+Fcnt];
		p=Amobj.Mmg*p;
		off.x*=k;
		off.y*=k;
		off.z*=k;
		p=off+p;
		p=~Amobj.Mmg*p;
		vv[j+Fcnt]=p;
		if((j+1) % Amobj.scnt  == 0) k++;
	}
				
	// mobj[i].op->Message(MSG_UPDATE);
	////PolyCnt コピー前ポリゴン数
	////mobj[i].Fcnt //選択ポリゴン数
	////UV情報をコピー

	Int32 PoCnt=   Amobj.Fcnt*CCnt;//コピーで増えたポリゴン数

	//UVと選択タグ更新
	if(PoCnt>0 && data.GetBool(MDATA_SELTAG))
	{
		if(m_uvTag || seltag)
		{

			//GePrint("  mobj[i].Ecnt*CCnt ="+String::IntToString(mobj[i].Ecnt*CCnt));
			//
			//PolyCnt:コピー前のポリゴン数
			//mobj[i].Ecnt*CCnt : コピーされたポリゴンの数
			//Int32 PoCnt=   mobj.Fcnt*CCnt;//コピーで増えたポリゴン数
			Char* map= NewMemClear(Char, PoCnt);//選択ポリゴン数分のマップを用意
			if(!map) return false;
               
		//	DP.printcheck("uv vc =",vc); 
			//増えたポリゴンごと処理
			for( Int32 n=0; n< CCnt; n++)
			{			
				for( Int32 nn=0; nn<Amobj.Fcnt; nn++)
				{
					//Int32 a=CheckNgonID(mobj[i].op, mobj[i].Enum[nn], PolyCnt, n+1, vc, PoCnt);
					Int32 a = CheckNgonID(Amobj.op, Amobj.Snum[nn], PolyCnt, n + 1, map, vc, PoCnt, Pbs->GetCount());
                    
					if(a != -1)
					{
						if( m_uvTag)
						{
							UVWHandle dataptr = m_uvTag->GetDataAddressW();
							UVWStruct uvw;
							UVWTag::Get(dataptr, Amobj.Snum[nn], uvw); //コピー元のUVを取得
							UVWTag::Set(dataptr, a, uvw); //コピー先にUVを割当
						}

						BaseTag* Btag = Amobj.op->GetFirstTag();
						Bool flag=false;
						//GePrint("PolyCnt="+String::IntToString(PolyCnt));
						while (Btag)
						{
							if (Btag->IsInstanceOf(Tpolygonselection))
							{
								//GePrint( Btag->GetName());

								seltag = (SelectionTag*)Btag;

								BaseSelect* bs = seltag->GetBaseSelect();
								Int32 ndx, seg = 0, smin, smax;
								while (bs->GetRange(seg++, PolyCnt, &smin, &smax))
								{
									for (ndx = smin; ndx <= smax; ndx++) // 記憶されたポリID
									{
									//	GePrint("ndx="+String::IntToString(ndx));
										if(ndx ==  Amobj.Snum[nn])
										{
											// GePrint("mobj.Snum[nn]="+String::IntToString(mobj.Snum[nn]));
											bs->Select(a); //コピー先にUVを割当
											flag=true;
											break;
										}
									}
									if(flag)
									{
										break;
									}
								}
							}

							if(flag)
							{
							//	GePrint("moo");
								break;
							}
							Btag = Btag->GetNext();
						}
					}
				}
			}
			DeleteMem(map);
			map=nullptr;

		}
	//		

//		if (data.GetBool(MDATA_SELTAG))
//		{
//		//	GePrint("mapcount =" + String::IntToString(PoCnt));
//			Char* map2 = NewMemClear(Char, PoCnt);//複製されたポリゴン数分のマップ
//			if (!map2) return false;
//
//		//	 DP.printcheck("seltag vc =",vc); 
//			BaseTag* Btag = mobj.op->GetFirstTag();
//
//			//GePrint("PolyCnt=" + String::IntToString(PolyCnt));
//			//GePrint(" CCnt=" + String::IntToString(CCnt));
//
//			//CCnt:コピーするカウント数
//			while (Btag)
//			{
//
//				if (Btag->IsInstanceOf(Tpolygonselection))
//				{
//					//セレクトタグから選択ポリゴンリストを取得
//					SelectionTag* seltag = (SelectionTag*)Btag;
//					BaseSelect* bs = seltag->GetBaseSelect();
//					Int32 ndx, seg = 0, smin, smax;
////					Int32 selcnt = bs->GetCount();
//					while (bs->GetRange(seg++, PolyCnt, &smin, &smax))
//					{
//						for (ndx = smin; ndx <= smax; ndx++) // 記憶されたポリID
//						{
//						//	GePrint(" ndx = " + String::IntToString(ndx));
//							for (Int32 n = 0; n < CCnt; n++) //増えたポリゴン数分処理
//							{
//							//	GePrint(" ndx = " + String::IntToString(ndx));
//						//		GePrint("--------------- CheckNgonID --------------");
//								Int32 a = CheckNgonID(mobj.op, ndx, PolyCnt, n+1, map2, vc, PoCnt, Pbs->GetCount());
//
//								if (a != -1)
//								{
//									bs->Select(a); //コピー先にUVを割当
//
//								}
//							}
//						}
//					}
//
//				}
//				Btag = Btag->GetNext();
//			}
//
//
//		////	GePrint("------map kakuninn-----------");
//		//	for(Int32 j=0; j< PoCnt; j++)
//		//	{
//		//		if(map2[j]) GePrint(String::IntToString(j)+"=true");
//		//		else   GePrint(String::IntToString(j)+"=false");
//		//	}
//			DeleteMem(map2);
//			map2 = nullptr;
//		}
	}
//

	Amobj.op->Message(MSG_UPDATE);
	Poly->VBOFreeUpdate();

	return true;
}


/*
  ポリゴンを移動
  ＊Shiftキー併用でポリゴンをカット＆ペーストまたはコピー＆ペースト
*/
Bool SnapAlign::ActionPolyCutMove(BaseDocument* doc, BaseContainer& data, Mobj Amobj, Vector vc)
{

	
   	DTOOL DP;
	//
	PointObject* pp=ToPoint(Amobj.op);
	if(!pp) return false;
	PolygonObject* Poly=ToPoly(Amobj.op);
	PointObject* point1 = ToPoint(Amobj.op);
	Vector* vv1 = point1->GetPointW();
	Int32 PolyCnt=Poly->GetPolygonCount();
	Matrix mg1 = Amobj.op->GetMg();

	Int32 ngonIdx = NOTOK;
	Pgon* ngonData = NULL;
	Int32 **ngons = NULL;
	Int32	 lNgonCount, *polymap = nullptr;
	DeleteMem(polymap);
	NgonBase* ngBase = Poly->GetNgonBase();
	if (ngBase)
	{
		ngonData = Poly->GetAndBuildNgon();

	}
	if (!Poly->GetPolygonTranslationMap(lNgonCount, polymap))
	{
		DeleteMem(polymap);
		return false;
	}

	if (!Poly->GetNGonTranslationMap(lNgonCount, polymap, ngons))
	{
		DeleteMem(polymap);
		DeleteMem(ngons);
		return FALSE;
	}

	AutoAlloc<Modeling> mod0;
	if (!mod0 || !mod0->InitObject(Amobj.op)) return false;



	BaseSelect* bs1 = Poly->GetPolygonS();

	CPolygon* CP1 = Poly->GetPolygonW();
	Int32 ndx, seg = 0, smin, smax;

	AutoAlloc<Modeling> mod;
	if (!mod || !mod->InitObject(_Starget1)) return false;

	Matrix mg2 = _Starget1->GetMg();
	PointObject* point2 = ToPoint(_Starget1);
	Int32 tgCnt = point2->GetPointCount();//ターゲットのポイント数
	PolygonObject* Poly2 = ToPoly(_Starget1);
	Int32 Poly2Cnt = Poly2->GetPolygonCount();//ターゲットのポリゴン数


	Int32 addPcnt = 0;

	Int32 ngoncnt = Poly->GetNgonCount();

	//UV処理
	//ソースのUVタグを取得
	UVWTag *m_uvTag = (UVWTag*)Amobj.op->GetTag(Tuvw);
	UVWTag *TG_uvTag = NULL;
	if (m_uvTag)
	{
		//ターゲットのUVタグを取得
		TG_uvTag = (UVWTag*)_Starget1->GetTag(Tuvw);
		//UVタグがない場合はタグを追加
		if (!TG_uvTag)
		{
			BaseTag* Utag= _Starget1->MakeTag(Tuvw, 0);
			TG_uvTag = (UVWTag *)Utag;
		}

	}


	doc->AddUndo(UNDOTYPE_CHANGE, _Starget1);

	Char* cmap = NewMemClear(Char, PolyCnt);
	if (!cmap)return false;
	Ngon ngon;

	for (Int32 j = 1; j < data.GetInt32(MDATA_BOOL_COPYCNT) + 1; j++)
	{
		seg = 0;
		vc *= j;

		while (bs1->GetRange(seg++, PolyCnt, &smin, &smax))
		{
			for (ndx = smin; ndx <= smax; ndx++)
			{
				if (!cmap[ndx])
				{

					ngonIdx = NOTOK;
					if (ngBase)
					{
						ngonIdx = ngBase->FindPolygon(ndx);

					}
					if (ngonIdx == NOTOK)
					{

						Int32 n = 3;
						if (CP1[ndx].c != CP1[ndx].d) n = 4;
						Vector* v1 = NewMemClear(Vector, n);
						if (!v1) return false;

						//ポイント座標取得
						v1[0] = vv1[CP1[ndx].a];
						v1[1] = vv1[CP1[ndx].b];
						v1[2] = vv1[CP1[ndx].c];
						if (n == 4) v1[3] = vv1[CP1[ndx].d];

						Int32* sid = NewMemClear(Int32, n);
						if (!sid) return false;
						for (Int32 i = 0; i < n; i++)
						{
							Vector v = mg1*v1[i];
							v += vc;
							sid[i] = mod->AddPoint(_Starget1, ~mg2*v); if (!sid[i]) return false;
						}

						Int32 p = mod->CreateNgon(_Starget1, sid, n);
						if (!p)return false;
						DeleteMem(v1);
						DeleteMem(sid);
						//smap[cnt] = ndx;
						//cnt += 1;
						addPcnt += n;
						cmap[ndx] = true;
					}
					else
					{

						Int32 x;

						for (x = 0; x < ngoncnt; x++)
						{
							if (ngons[x][1] == ndx)
							{

								for (Int32 y = 1; y <= ngons[x][0]; y++)
								{
									Int32 a = ngons[x][y];
									cmap[a] = true;
								}

								if (!mod0->GetNgon(Amobj.op, ndx, &ngon)) return false;
								Int32 * pv = ngon.points;

								Int32* sid = NewMemClear(Int32, ngon.count);
								if (!sid) return false;
								for (Int32 i = 0; i < ngon.count; i++)
								{
									Vector v = mg1* vv1[pv[i]];

									v += vc;
									sid[i] = mod->AddPoint(_Starget1, ~mg2*v); if (!sid[i]) return false;
								}
								Int32 k = mod->CreateNgon(_Starget1, sid, ngon.count);
								if (!k)return false;
								addPcnt += ngon.count;
								DeleteMem(sid);
							}
						}
					}
				}
			}
		}
		for (Int32 i = 0; i < PolyCnt; i++)
		{
			cmap[i] = false;
		}
	}

	if (!mod->Commit(_Starget1, MODELING_COMMIT_UPDATE)) return false;



	//生成したポリゴン分頂点連結
	AutoAlloc<Modeling> mod2;
	if (!mod2 || !mod2->InitObject(_Starget1)) return false;

	Vector* v2 = point2->GetPointW();

	//結合処理
	for (Int32 i = 0; i < addPcnt-1; i++)
	{
		Int32 w = i + tgCnt;
		for (Int32 j = i + 1; j < addPcnt; j++)
		{
			Int32 t = j + tgCnt;
			if (Len(v2[w]- v2[t]) < 0.01)
			{
				mod2->WeldPoints(_Starget1, w, t);
			}
		}
	}


	//頂点結合オプション有効
	if (data.GetBool(MDATA_OPTIMIZE))
	{


		Int32 tgCnt2 = point2->GetPointCount();//ターゲットのポイント数
		for (Int32 i = tgCnt; i < tgCnt2; i++)
		{
			for (Int32 j = 0; j < tgCnt; j++)
			{
				//if (v2[i] == v2[j])
                if (Len(v2[i]- v2[j]) < 0.01)
				{
					mod2->WeldPoints(_Starget1, j, i);
				}
			}
		}

	}
	
	if (!mod2->Commit(_Starget1, MODELING_COMMIT_UPDATE)) return false;

	if (m_uvTag && TG_uvTag)
	{
		void *dataptr = m_uvTag->GetDataAddressW();
		void *TGdataptr = TG_uvTag->GetDataAddressW();

		Int32 adcnt = 0;
		for (Int32 j = 1; j < data.GetInt32(MDATA_BOOL_COPYCNT) + 1; j++)
		{

			//UVデータ処理
			seg = 0;

			while (bs1->GetRange(seg++, PolyCnt, &smin, &smax))
			{
				for (ndx = smin; ndx <= smax; ndx++)
				{
					if (!cmap[ndx])
					{
						ngonIdx = NOTOK;
						if (ngBase)
						{
							ngonIdx = ngBase->FindPolygon(ndx);

						}
						if (ngonIdx == NOTOK)
						{
							//UVWデータをコピー
							UVWStruct uvw;
							UVWTag::Get(dataptr, ndx, uvw);

							Int32 nid = Poly2Cnt + adcnt;

					
							UVWTag::Set(TGdataptr, nid, uvw);
							adcnt += 1;
							cmap[ndx] = true;
						}
						else
						{

							Int32 x;
							//						Int32 check = -1;
							for (x = 0; x < ngoncnt; x++)
							{
								if (ngons[x][1] == ndx)
								{

									for (Int32 y = 1; y <= ngons[x][0]; y++)
									{
										Int32 a = ngons[x][y];

										UVWStruct uvw;
										UVWTag::Get(dataptr, a, uvw);

										Int32 nid = Poly2Cnt + adcnt;

										UVWTag::Set(TGdataptr, nid, uvw);
										adcnt += 1;

										cmap[a] = true;
									}
								}
							}
						}
					}
				}
			}
			for (Int32 i = 0; i < PolyCnt; i++)
			{
				cmap[i] = false;
			}
		}

	}


	BaseTag* t0=GetLastTag( doc, _Starget1);

	if(t0)
	{

		BaseTag* Stag = _Starget1->MakeTag(Tpolygonselection,t0);
		if (Stag)
		{
			
			SelectionTag* Seltag = (SelectionTag*)Stag;
			Int32 time = GeGetTimer();
			String name = Amobj.op->GetName() + "_" + String::IntToString(time);
			Seltag->SetName(name);
			BaseSelect* sbs = Seltag->GetBaseSelect();
			Int32 ccnt = Poly2->GetPolygonCount();

			for (Int32 i = Poly2Cnt; i<ccnt; i++)
			{
				sbs->Select(i);
			}

			TextureTag* ttag= (TextureTag*)Amobj.op ->GetTag(Ttexture,0);
			if(ttag)
			{
				BaseMaterial* mat= ttag->GetMaterial();
				if(mat)
				{
					BaseTag* t=GetLastTag( doc, _Starget1);
					 if(t)
					 {
						TextureTag* Ttag=(TextureTag*)_Starget1->MakeTag(Ttexture,t);
						if(Ttag)
						{
							Ttag->SetMaterial(mat);
							Ttag->SetParameter(DescID(TEXTURETAG_RESTRICTION), GeData(name), DESCFLAGS_SET_0);
							Ttag->SetParameter(DescID(TEXTURETAG_PROJECTION), GeData(TEXTURETAG_PROJECTION_UVW), DESCFLAGS_SET_0);
						}
					 }


				}
			}

			_Starget1->Message(MSG_UPDATE);

		}
	}




	DeleteMem(cmap);

	_Starget1->Message(MSG_UPDATE);

	//ソースの選択ポリゴンを消去
	if (data.GetInt32(MDATA_ACTIONLIST) == CH_CUTPASTE)
	{
	//	doc->AddUndo(UNDOTYPE_CHANGE, mobj.op);
		ModelingCommandData    gmcd2;
		gmcd2.doc = NULL;
		gmcd2.op = Amobj.op;
		gmcd2.bc = NULL;
		gmcd2.mode = MODELINGCOMMANDMODE_POLYGONSELECTION;
		gmcd2.flags = MODELINGCOMMANDFLAGS_0;
		if (!SendModelingCommand(MCOMMAND_DELETE, gmcd2)) return false;
  
        BaseContainer bc;
        bc.SetFloat(MDATA_OPTIMIZE_TOLERANCE ,0);
        bc.SetBool(MDATA_OPTIMIZE_POINTS, FALSE);
        bc.SetBool(MDATA_OPTIMIZE_POLYGONS, FALSE);
        bc.SetBool(MDATA_OPTIMIZE_UNUSEDPOINTS, TRUE);
        
        ModelingCommandData    gmcd3;
        gmcd3.doc = NULL;
        gmcd3.op =  Amobj.op;
        gmcd3.mode = MODELINGCOMMANDMODE_POLYGONSELECTION;
        gmcd3.flags = MODELINGCOMMANDFLAGS_0;
        gmcd3.bc = &bc;
        if (!(SendModelingCommand(MCOMMAND_OPTIMIZE, gmcd3))) return false;
		Amobj.op->Message(MSG_UPDATE);

	}
	 

	return true;
}

BaseTag*  SnapAlign::GetLastTag(BaseDocument* doc, BaseObject* op)
{
  if(!op) return nullptr;

  BaseTag* tag=op->GetFirstTag();
  BaseTag* Atag = nullptr;
  while(tag)
  {
	 
	  Atag=tag;
	  tag=tag->GetNext();
  }

  return Atag;
}

/////////////////////////////////////////////////////////////////
Bool RegisterSnapAlign()
{

	Filename fn;
	fn = GeGetPluginPath() + "res" + "DP_PBTN.tif";
	AutoAlloc<BaseBitmap> bmp;
	if (IMAGERESULT_OK != bmp->Init(fn)) return false;
	RegisterIcon(ID_BITMAPBTN_SAVE, bmp, 0 * 16, 0, 16, 16, ICONFLAG_COPY);

	String name=GeLoadString(IDS_SNAPTRANSFER );
	return RegisterToolPlugin(ID_MODELING_SnapALIGN, name, (PLUGINFLAG_TOOL_TWEAK_NO_HIGHLIGHT | PLUGINFLAG_TOOL_EDITSTATES | PLUGINFLAG_TOOL_SNAPSETTINGS
		), AutoBitmap("SnapPosition.tif"), GeLoadString(IDS_HLP_SNAPTRANSFER), NewObjClear(SnapAlign));

}

  	//Int32 ngonIdx = NOTOK;
	//Pgon* ngonData = NULL;
	//NgonBase* ngBase = mesh->GetNgonBase();
	//if (ngBase)
	//  ngonData = mesh->GetAndBuildNgon();

	  // if ngBase there are some ngons in the mesh
	//ngonIdx = NOTOK;
 //  
	//if (ngBase)
	//{
	//	ngonIdx = ngBase->FindPolygon(num);
	//}
	// if ngonIdx != NOTOK poly is a part of ngonIdx Ngon
	//if (ngonIdx == NOTOK)
	//{
	// Standard polygon
		//	GePrint("----------------");
		//for(Int32 i=0; i< CNT; i++)
		//{

		//	if(!map[i])
		//	{
		//		if(	HikakuShape(vertices, polygons, num, i+Cnt, v))
		//		{
		//			id=	i+Cnt;
		//			GePrint(" id-Cnt ="+String::IntToString(id-Cnt));
		//			map[id-Cnt]=true;
		//			return id;
		//		}
		//	}
		//}
		//	GePrint("----------------");
		//id=Cnt+k;
	//}
//	else //Ngonの場合
//	{
//		Int32	 lNgonCount, *polymap = nullptr;
//		Int32 **ngons = nullptr;
//		DeleteMem(polymap);
////		Int32 polycnt = mesh->GetPolygonCount();
//		if (!mesh->GetPolygonTranslationMap(lNgonCount, polymap))
//		{
//			DeleteMem(polymap);
//			return 	false;
//		}
//		//Ngonのリストを作成
//        if(!mesh->GetNGonTranslationMap(lNgonCount, polymap, ngons))
//        {
//            DeleteMem(polymap);
//			DeleteMem(ngons);
//            return false;
//        }
//		Int32 ngoncnt = mesh->GetNgonCount(); //Ngonの数
//
//		for(Int32 x=0; x< ngoncnt;x++)
//		{
//
//			Int32 y;
//			for(y=1; y<=ngons[x][0]; y++)
//			{
//				if(Cnt <= ngons[x][y])
//				{
//					if(!map[ngons[x][y]])
//					{
//						 //ポイント座標でポリゴン形を確認
//						if(	HikakuShape(vertices, polygons, num, ngons[x][y], v))
//						{
//							id=	ngons[x][y];
//							DeleteMem(polymap);
//							DeleteMem(ngons);
//
//							map[id-Cnt]=true;
//							return id;
//						}
//					}
//				}
//			}
//		}
//
//		DeleteMem(polymap);
//		DeleteMem(ngons);
//	}
