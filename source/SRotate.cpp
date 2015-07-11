// SRotate.cpp
//////////////////////////////////////////////////////////////////////

#include "c4d.h"

//#include "dPobjects.h" //resフォルダのヘッダーファイルをインクルード
//#include "operatingsystem.h"
#include "Dtool.h"




#define ID_SRotate	1024789


//void printcheck(String st, Vector p)
//{
//	GePrint(st + ".x=" + String::FloatToString(p.x) + st + ".y=" + String::FloatToString(p.y) + st + ".z=" + String::FloatToString(p.z));
//}
//////////////////////////////////////////////////////////////////
//クラス定義
class SnapRot : public DescriptionToolData
{
private:
	Vector G_markpoint; //ポインター座標

	Vector G_P1;		//１点目のポイント
	Vector G_P2;		//2点目のポイント
	Vector G_P3;		//3点目のポイント
	Int32 G_STEP;		//進行ステップ
	Int32 G_SnapFlag;  //スナップ判定
    
    Vector G1;
    Vector CenterPos;

	Vector G_nvec;    //法線ベクトル
	Bool CtrlKey;		//Ctrlキー判定
	Mobj *mobj;

	Float rn, rp;
	Int32 ObjCnt;

	Bool getFlag;
	Bool AltKey;
	Bool PShift;


	Bool PlaneSet_BTNFlag;
	Bool PlaneSet_Flag;

	Int32 PlaneSetCnt;
	Vector PlV[3];

	Int32 flagR, flagRB;


	Float _RADROT;

    Bool Drag;
	Int32 Edge2Line_Step;
	Vector EdgeLineVectorA[2];
 	Vector EdgeLineVectorB[2];
	Vector CircleCenter;
	Int32 PlaneNum;

	BaseObject* _Starget;
	Int32 SnapID;
	Float HUDOffset;

	Bool checkFlag;

	Float _MX; //マウスカーソル座標(２D)
	Float _MY; //マウスカーソル座標(２D)



	Bool InitFlag; //起動時に一度だけmainスナップ設定を行うフラグ


	Matrix FaceMg;		//ポリゴン法線マトリックス
 	Matrix EndMg;		//最終マトリックス


	Vector ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection);
//	Bool SetDPlane(BaseDocument *doc, BaseDraw* bd, BaseContainer &data, Bool flag, Int32 num);
	Bool RectPointVector( BaseObject *op, Int32 i );
	Bool SetPointAngle( BaseObject *op, Int32 i, Matrix mr );
	Bool RectPolygonVector(BaseObject *op, Int32 i );
	Bool RectEdgeVector(BaseObject *op, Int32 i );
	Bool AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE type);
	//Bool SetPlaneAxis(BaseDocument* doc);
	Bool ResetPlaneAxis(BaseDocument* doc);
	Bool SetAngle(BaseDocument* doc, BaseContainer& data);
	Bool GetSELPoly(BaseObject *op, Int32 num, Int32 id);
	Bool GetSELEdge(BaseObject *op, Int32 num, Int32 id);
	Bool RectPointALLVector(BaseObject *op, Int32 i );
	Bool InteractiveValue(BaseDocument *doc, BaseContainer &data);	
	void ResetTool();
	Bool Set3pointPlane(BaseDocument* doc, BaseContainer &data );
	Bool ResetPlaneAxis(BaseDocument* doc, BaseContainer &data);
	//Bool SnapSync(BaseDocument* doc, BaseContainer& data);
	void Priinit(BaseContainer& data, Bool flag );
	void SaveData(HyperFile* hf, BaseContainer& data);
	void DefoultSet( BaseContainer& data);
	void ReadData(BaseContainer bc, BaseContainer& data);
	Bool SetDrawExtubeAngle(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, Int32 i, Matrix mr, Float rad );

	Bool ExtubePoly(BaseDocument* doc, BaseContainer& data, Int32 i );
//	void TEST(BaseDocument* doc );
	Vector Rad2Vector( BaseObject* op, Vector vp,Vector Center,  Float cr, Matrix cmg );
    Bool  Edge2CenterPos(BaseDocument* doc , BaseContainer& data);



public:
	SnapRot(); //コンストラクタ
	virtual ~SnapRot(); //デストラクタ
	virtual Int32 GetToolPluginId() { return ID_SRotate; }
	virtual const String GetResourceSymbol() { return String("SnapRotate"); } //resファイルで記述されている名前を入れる
	virtual Bool InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt);
	virtual void FreeTool( BaseDocument* doc, BaseContainer& data) ;
	virtual Int32 GetState(BaseDocument *doc);
	virtual Bool MouseInput( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg) ;
	virtual TOOLDRAW Draw(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, BaseThread* bt, TOOLDRAWFLAGS flags);
	virtual Bool GetCursorInfo(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, Float x, Float y, BaseContainer& bc);
	virtual void InitDefaultSettings(BaseDocument *doc, BaseContainer &data);
	virtual Bool DoCommand( ModelingCommandData &mdat);
	virtual Bool GetDEnabling(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_ENABLE flags, const BaseContainer* itemdesc);
	virtual Bool Message(BaseDocument* doc, BaseContainer& data, Int32 type, void* t_data);
	virtual Bool KeyboardInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg);
	virtual Bool SetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags);
	virtual Bool GetDDescription(BaseDocument* doc, BaseContainer& data, Description* description, DESCFLAGS_DESC& flags);
	virtual Bool GetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags);

	//Bool SetSizeDCube(BaseContainer *bc,Vector *ve, Vector *ppos, BaseObject *op, BaseContainer &data);
	private:
		SnapCore* _snap;			// the snap object for this tool
};

////////////////////////////////////////////////////////////////////////

SnapRot::SnapRot()
{
	G_markpoint=Vector();

	EndMg = Matrix();
	G_P1=G_P2=G_P3=Vector();

	G_STEP=NULL;
	G_SnapFlag=NULL;
	G_nvec=Vector();
	CtrlKey=false;
	mobj=nullptr;

	ObjCnt=0;
	rn=0.0;
	rp=0.0;
	
	getFlag=false;
	AltKey=false;


	//GePrint("--------------------------");
	//GePrint("SnapRotate Version 1.23");
	//GePrint("copyright(c) Since 2009 anoano All rights reserved.");
	//GePrint("--------------------------");

	PlaneSet_BTNFlag=false;
	PlaneSetCnt=0;
	PlaneSet_Flag=false;
	PlV[0]=PlV[1]=PlV[2]=Vector();

	flagR=0;
	flagRB=0;
    Drag=false;

	Edge2Line_Step=0;
	PlaneNum=IDE_XZ;

	HUDOffset=40;

	InitFlag= false;
	_snap = nullptr;
}
//デストラクタ
SnapRot::~SnapRot()
{
	if (_snap)
		SnapCore::Free(_snap);
	_snap = nullptr;
}
//--------------------------------------------------------------------------------------------------------------------------
//tool終了時実行
void SnapRot::FreeTool( BaseDocument* doc, BaseContainer& data)
{
	if(doc)
	{
		BaseObject* plane= GetWorkplaneObject( doc);
		Matrix mg;
		plane->SetMg(mg);
		PlaneSet_Flag=false;
		PlaneSet_BTNFlag=false;
		PlaneSetCnt=0;
		PlV[0]=PlV[1]=PlV[2]=Vector();
		ResetTool();
        Edge2Line_Step=0;
        data.SetInt32(IDE_Snap_AXIS,IDE_XZ);
		BaseDraw* bd = doc->GetActiveBaseDraw();
		GeData vd = bd->GetParameterData(BASEDRAW_DATA_PLANE_LEGACY_MODE);
		vd.SetInt32(false);
		bd->SetParameter(BASEDRAW_DATA_PLANE_LEGACY_MODE, vd, DESCFLAGS_SET_USERINTERACTION);
		InitFlag= false;

	}

	DescriptionToolData::FreeTool( doc, data);
}
//--------------------------------------------------------------------------------------------------------------------------
//最初期設定
void SnapRot::InitDefaultSettings( BaseDocument *doc, BaseContainer &data)
{ 
	rn=rp=0.0;

//**************************************************
	DTOOL DP;
	DP.init(doc, data);
	Priinit( data, false );
//**************************************************
	ResetTool();
	if (!DP.SnapSync(doc, data, PlaneSet_BTNFlag)) return;

	DescriptionToolData::InitDefaultSettings( doc, data);
}


//--------------------------------------------------------------------------------------------------------------------------
//スナップ項目表示設定
Bool SnapRot::GetDEnabling(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_ENABLE flags, const BaseContainer* itemdesc)
{
	if (!doc)     return false;
	
	//スナップスイッチの状況により表示の有効/無効設定
	switch (id[0].id)
	{
		case MDATA_SETAXIS:
			{
				if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc) || PlaneSet_Flag || G_STEP) return false;
				else if(	Edge2Line_Step != 0 &&  G_STEP==0 ) return false;
				else return true;
			}
		case MDATA_RESET:
			{
				if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc) || G_STEP) return false;
				else return true;
			}
		case MDATA_DRAG_IDE_OBJ_TARGET:
			{
				if( PlaneSet_Flag || G_STEP ) return false;
				else return true;
			}
		case IDB_BTN_SET:
			{
				if(G_STEP==2) return true;
				else return false;
			}
		case IDE_Snap_AXIS:
			{
				Int32 view = doc->GetActiveBaseDraw()->GetProjection();
				if (G_STEP)
				{
					return false;
				}
				else if (view > 1 && view < 8)
				{
					return false;
				}
				else return true;
				
			}
		//*********************************************************

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
			if(data.GetBool(SNAP_GUIDE_SNAPG)&& data.GetBool(SNAP_ENABLEG) && data.GetBool(SNAP_DYNAMICGUIDE_SNAPG))
			{
				return (SNAP_PERPENDICULARG >= 0L);
			}
			else
			{
				return ( SNAP_PERPENDICULARG <= 0L);
			}
		}
	case MDATA_CROSSPOSBTN:
		{
			if(	Edge2Line_Step == 0 && !PlaneSet_Flag && G_STEP==0 ) return true;
			else return false;
		}
	case IDE_MODE:
		{
			if( data.GetInt32( MDATA_SELECTMODE) ==  CH_ROT && G_STEP==0 ) return true;
			else return false;

		}

	case MDATA_SELECTMODE:
		{
			if(  G_STEP==0) return true;
			else return false;
		}
	case MDATA_EXTUBESUBDEV:
		{
			if(data.GetInt32( MDATA_SELECTMODE) == CH_EXTUBE) return true;
			else return false;
		}
	default:
		break;
     }

	return SUPER::GetDEnabling(doc, data, id, t_data, flags, itemdesc);
}
//--------------------------------------------------------------------------------------------------------------------------
//現状取得
Int32 SnapRot::GetState(BaseDocument *doc)
{
	//	Int32 ac=doc->GetAction();
	//if(ac != 1023672) GePrint("ac=");
	//GePrint("st");
	return CMD_ENABLED;
}
//--------------------------------------------------------------------------------------------------------------------------
//初期化
Bool SnapRot::InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt)
{
   	DTOOL DP;
    SYSTEMINFO    stype =        GeGetSystemInfo();
	if ( stype & (SYSTEMINFO_SAVABLEDEMO | SYSTEMINFO_SAVABLEDEMO_ACTIVE) || stype & SYSTEMINFO_COMMANDLINE)
	{

	}
	else
	{
		//シリアルチェック
		DTOOL DP;
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

void SnapRot::ResetTool()
{

	G_STEP = 0;
	G_P1=G_P2=G_P3=Vector();

	rn=rp=0.0;

	if(ObjCnt)
	{
		for(Int32 i=0; i<ObjCnt; i++)
		{
			if(mobj[i].vv)DeleteMem(mobj[i].vv);
			if( mobj[i].pnum) DeleteMem(mobj[i].pnum);
			if(mobj[i].Enum)  DeleteMem(mobj[i].Enum);
			if(mobj[i].Snum) DeleteMem(mobj[i].Snum);
			if(mobj[i].emap) DeleteMem(mobj[i].emap);
			
		}
	}
	DeleteMem(mobj);
	mobj=nullptr;
	ObjCnt=0;
    Edge2Line_Step=0;
}
//--------------------------------------------------------------------------------------------------------------------------
//モデリングコマンド
Bool SnapRot::DoCommand( ModelingCommandData &mdat)
{
	//return ModelingAct_for$MCommand( &mdat) ;
		return true;
}

//--------------------------------------------------------------------------------------------------------------------------
//キーインプット
Bool SnapRot::KeyboardInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg)
{
	//static Int32 fl=0;
	Int32 key = msg.GetData(BFM_INPUT_CHANNEL).GetInt32();
	if(key == KEY_ESC)
	{
        for(Int32 i=0; i<ObjCnt; i++)
        {
            PointObject* point=ToPoint(mobj[i].op);
            Vector* vv=point->GetPointW();
            for(Int32 j=0; j<mobj[i].scnt; j++)
            {
                Vector v=mobj[i].vv[j]; //ポイント座標取得
                vv[mobj[i].pnum[j]]=v;
            }
            mobj[i].op->Message(MSG_UPDATE);
        }


		if(Edge2Line_Step != 0)
		{
		   Edge2Line_Step = 0;
		}
        
		ResetTool();
        
	}
	EventAdd();
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------
//マウスインプット
Bool SnapRot::MouseInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd,EditorWindow *win, const BaseContainer &msg)
{
	//DPTOOL DP;
	if(!doc) return false;
	DTOOL DP;
	
	//最初のクリック位置（スクリーン座標）の取得	
	Float mx = msg.GetFloat(BFM_INPUT_X);
	Float my = msg.GetFloat(BFM_INPUT_Y);

	Int32 button=NULL;
	//クリックボタンの取得
	switch (msg.GetInt32(BFM_INPUT_CHANNEL))
	{
		case BFM_INPUT_MOUSELEFT : button=KEY_MLEFT; break;
		case BFM_INPUT_MOUSERIGHT: button=KEY_MRIGHT; break;
		default: break;
	}
	//左ボタン以外は無効
	if(button!=KEY_MLEFT) return false;

	if(PlaneSet_BTNFlag)
	{
		if(G_SnapFlag)
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
					PlaneSetCnt=0;
					PlaneSet_Flag=true;
					PlaneSet_BTNFlag=false;

				}
			}
			else if(PlaneSetCnt==1)
			{
				if( G_SnapFlag == 1 )
				{
					PlV[1]=G_markpoint;
					PlaneSetCnt++;
				}
			}
			else if(PlaneSetCnt==2)
			{
				if( G_SnapFlag == 1 )
				{
					PlV[2]=G_markpoint;
                    if(!DP.Get3PointNormMg(PlV[0],PlV[1],PlV[2])) return  ResetPlaneAxis(doc,data);
                    //変更した作業平面からマトリックスを取得
                    BaseObject* plane = GetWorkplaneObject(doc);
                    FaceMg = plane->GetMg();
                    PlaneSetCnt=0;
                    PlaneSet_Flag=true;
                    PlaneSet_BTNFlag=false;
				}
			}

		}
		//BaseObject* plane = GetWorkplaneObject(doc);
		//Vector r = MatrixToHPB(plane->GetMg(), ROTATIONORDER_DEFAULT);
		//DP.printcheckR(" Input Plane Rot=", r);

		//**********************************************
	}
	else if(Edge2Line_Step != 0)
	{
		 if( Edge2Line_Step ==1 )
		 {
			Edge2Line_Step++;
		 }
		 else if( Edge2Line_Step ==2 )
		 {

			G_P1=CircleCenter;
			G_STEP = 1;
			Edge2Line_Step = 0;
		 }
		 	
	}
	else
	{
        G1=G_markpoint;
        Float dx,dy;
        
		BaseContainer bc;
		BaseContainer device;
		Drag=false;
		doc->StartUndo();
		////マウスドラッグスタート
		win->MouseDragStart(button,mx,my,MOUSEDRAGFLAGS_DONTHIDEMOUSE|MOUSEDRAGFLAGS_NOMOVE);
		while (win->MouseDrag(&dx,&dy,&device)==MOUSEDRAGRESULT_CONTINUE)		//ドラッグ距離の取得
		{
            if( G_STEP == 0)
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
               // G_markpoint=DP.SnapMouseVector( doc, data, bd, mx, my, Vector(), EndMg, &G_SnapFlag,  G_STEP, &G_P1, &G_P2, &G_nvec,  PlaneSet_BTNFlag, &FaceMg, &_Starget, &SnapID  );
				G_markpoint=DP.SnapMouseVector(doc, data, bd, _snap, mx, my, EndMg, &G_SnapFlag, G_STEP, G_markpoint, G_P1, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
                AutoAlloc<SnapCore> snap;
                if (!snap) return false;
                
                snap->Init(doc, bd);

                Drag=true;
                SnapResult Snapres;
                if(IsSnapEnabled(doc))
                {
                    if (snap->Snap(G_markpoint,Snapres))
                    {
                        G_markpoint += Snapres.delta;//if pos can be snapped it is set accordingly
                        G_SnapFlag= 1;

                        if(data.GetInt32(SNAP_METHODG) ==SNAP_2DG )
                        {
							G_markpoint= DP.ReMgVector(bd, EndMg, G_markpoint);
                        }
                    }
                    else
                    {
                        G_SnapFlag=0;
						G_markpoint= DP.ReMgVector(bd, EndMg, G_markpoint);                 
                    }
                }
                else
                {
                    G_SnapFlag=0;
					G_markpoint= DP.ReMgVector(bd, EndMg, G_markpoint);
                }
            }

            DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
            
		}
        if (win->MouseDragEnd()==MOUSEDRAGRESULT_FINISHED)
		{
            if(G_STEP==0){
                //一点目のポイント座標を取得
                if(Drag){
                   G_P1= (G1+G_markpoint)/2;
                }else{
                    G_P1=Vector();
                    G_P1=G_markpoint;
                }
                G_STEP = 1;

				 
            }
            else if(G_STEP == 1)
            {
			
				////選択オブジェクトの取得
                AutoAlloc <AtomArray> arr;
                if (!doc || !arr) goto Error;
                 
                //現在のモードを取得
                Int32 mode = doc->GetMode();
                if( mode== Mpoints || mode == Medges || mode == Mpolygons )
                {
                    //doc->GetActivePolygonObjects(*arr, true);
                    doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
                    Int32 cnt=arr->GetCount(); //選択オブジェクト数を取得
                    	
                    if(cnt)
                    {
                        
                        
                        Int32 gh=0;
                        for(Int32 i=0; i<cnt; i++)
                        {
                            BaseObject *op=(BaseObject *)arr->GetIndex(i);
                            if(!op) goto Error;
                            if(op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd)
                            {
                                gh++;
                            }
                        }
                        if(gh==0) goto Error;

                        AddUndo(doc, arr, UNDOTYPE_CHANGE);
                        mobj=nullptr;
                        //選択ﾎﾟｲﾝﾄを格納用変数

                        //mobj=(Mobj *) GeAlloc(gh * sizeof(Mobj));
                         mobj= NewMemClear(Mobj, gh);
                        if(!mobj)  goto Error;

                           
                        gh=0;
                        for(Int32 i=0; i<cnt; i++)
                        {
                            BaseObject *op=(BaseObject *)arr->GetIndex(i);
                            
                            if(op && (op->GetType() == Opolygon || op->GetType() == Ospline|| op->GetType() == Offd))
                            {
                                mobj[i].op=op;
                                if(mode== Mpoints)
                                {
									 //	 GePrint(" aaaa"); 
                                    if(!RectPointVector( op, gh)) goto Error;
                                }
                                else if(mode== Medges)
                                {
									 //	 GePrint("   bbbb"); 
                                    if(!RectEdgeVector(op, gh )) goto Error;
                                }
                                else if( mode == Mpolygons )
                                {
									 //	 GePrint("   cccc"); 
                                    if(!RectPolygonVector(op, gh )) goto Error;
                                }
                                gh++;
                            }
                        }
                        ObjCnt=gh;
                        
                    }
                      
                }
                else if(mode == Mobject || mode == Mmodel )
                {
                    doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
                    Int32 cnt=arr->GetCount();
                    if(!cnt)  goto Error;

                    ObjCnt=cnt;
                    AddUndo(doc, arr, UNDOTYPE_CHANGE);
                    DeleteMem(mobj);
                    mobj=nullptr;

                    mobj=NewMemClear(Mobj, cnt);

                    if (!mobj )return false;
                        //    throw 1;

                    for(Int32 i=0; i<cnt; i++)
                    {
                        BaseObject *op=(BaseObject *)arr->GetIndex(i);
                    
                        mobj[i].Mmg=op->GetMg();
                        mobj[i].op=op;
                    }

                }

                G_P2=Vector();
                G_P2=G_markpoint;

                Int32 Smode = data.GetInt32(IDE_MODE);

                G_STEP = 2;
			//	GePrint("   G_STEP=" + String::IntToString(G_STEP));
                if(Smode == CH_2SNAP)
                {
                    SetAngle(doc,data);
                }

				//GePrint("            kkkkkkkkkkkkk");
				//GePrint("   *****G_STEP=" + String::IntToString(G_STEP));
			
            }
            else if(G_STEP == 2)
            {
				
                if(data.GetInt32(MDATA_SELECTMODE) == CH_EXTUBE  && ( doc->GetMode() == Medges ||  doc->GetMode() == Mpolygons) )
                {
                    if(ObjCnt)
                    {
                        for(Int32 i=0; i<ObjCnt; i++)
                        {
                            BaseObject *op=mobj[i].op;
                            if(!op) goto Error;
                            if(!ExtubePoly(doc,data, i )) goto Error;
                        }
                    }
                }
                ResetTool();
            }
        }
	}



	doc->EndUndo();

	EventAdd();
	return true;

	 
Error:
	 ResetTool();
	return false;
}

//--------------------------------------------------------------------------------------------------------------------------
//描画
TOOLDRAW SnapRot::Draw(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, BaseThread* bt, TOOLDRAWFLAGS flags)
{
	DTOOL DP;
	//BaseObject* plane = GetWorkplaneObject(doc);
	//Vector r = MatrixToHPB(plane->GetMg(), ROTATIONORDER_DEFAULT);
	//DP.printcheckR(" Draw  Plane Rot=", r);

	if (!(flags & TOOLDRAWFLAGS_HIGHLIGHT)) 
	return TOOLDRAW_0;

	Int32 Smode = data.GetInt32(IDE_MODE);
	
	DP.DispMarker(doc, bd, bh, data, EndMg, G_markpoint, PlaneSet_Flag, G_SnapFlag, PlaneSet_BTNFlag, FaceMg,  _MX,  _MY, 1);
	//Vector r = MatrixToHPB(FaceMg, ROTATIONORDER_DEFAULT);
	//DP.printcheckR(" Draw Rot=", r);
	GeData lw = bd->GetParameterData(DRAW_PARAMETER_LINEWIDTH);
	bd->SetMatrix_Matrix(nullptr, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->SetTransparency(0);
	
    if(G_STEP == 0 && Drag)	//ドラッグモード時の中点位置表示
    {
		bd->SetMatrix_Matrix(nullptr, bh->GetMg());
        Vector CenterV= (G1+G_markpoint)/2;

		bd->SetPen(Vector(0.0,1.0,0.5));
		bd->DrawLine(G1,G_markpoint, 0);
      
        bd->SetPen(Vector(1.0,0.0,0.5));
		bd->DrawHandle(G1,DRAWHANDLE_BIG,0); //１点目のポイント
 		bd->DrawHandle(CenterV,DRAWHANDLE_BIG,0); //１点目のポイント       
    }
	else if(G_STEP==1)
	{		
		bd->SetMatrix_Matrix(nullptr, bh->GetMg());
	//	GePrint("step=1");
		bd->SetPen(Vector(1.0,0.0,0.5));
		bd->DrawHandle(G_P1,DRAWHANDLE_BIG,0); //１点目のポイント

		bd->SetPen(Vector(0.0,1.0,0.5));
		bd->DrawLine(G_P1,G_markpoint, 0);
		//DP.printcheck("Draw :: G_markpoint = ", G_markpoint);

	}
	else if(G_STEP==2)
	{


		//bd->SetMatrix_Matrix(nullptr, bh->GetMg());
		//Vector CR = MatrixToHPB(BufMg, ROTATIONORDER_DEFAULT);
		//DP.printcheckR("BufMg=",CR);
		//Vector CR2 = MatrixToHPB(EndMg, ROTATIONORDER_DEFAULT);
		//DP.printcheckR("EndMg=", CR2);
		Matrix A_mg = EndMg;

		//Vector vpos = BufMg.off;
        
        //Vector CR = MatrixToHPB( EndMg, ROTATIONORDER_DEFAULT);
        //DP.printcheckR("EndMg=",CR);
		//マトリックスの位置をリセット
		A_mg.off = Vector();

		//Vector CR = MatrixToHPB(FaceMg, ROTATIONORDER_DEFAULT);
		//DP.printcheckR("FaceMg=", CR);

		//CR = MatrixToHPB(EndMg, ROTATIONORDER_DEFAULT);
		//DP.printcheckR("EndMg=", CR);


		if(Smode== CH_2SNAP) return TOOLDRAW_0;
		
		Vector spos=G_markpoint;
		if (G_SnapFlag != 0)
		{
			spos = G_P3;
		}


 		if(data.GetBool( MDATA_SW2D))
		{
			Vector p1=  G_P1;
			Vector p2 = G_P2;
			Vector p3 = spos;
			p1 =~A_mg*p1;
			p2=	~A_mg*p2;
			p3=	~A_mg*p3;

			p2.y=p1.y;
			p3.y=p1.y;

			G_P2=A_mg*p2;

			spos=A_mg*p3;

		}

		bd->SetMatrix_Matrix(nullptr, bh->GetMg());

		bd->SetPen(Vector(1.0,0.0,0.5));
		bd->DrawHandle(G_P1,DRAWHANDLE_BIG,0);
		bd->DrawHandle(G_P2,DRAWHANDLE_BIG,0);
		bd->DrawHandle(spos, DRAWHANDLE_BIG, 0);

		bd->SetPen(Vector(0.0,1.0,0.5));
		bd->DrawLine(G_P1,G_P2, 0);
		bd->DrawLine(G_P1, spos, 0);
	
		Vector rv1=G_P1;
		Vector rv2=G_P2;
		Vector rv3 = spos;
		//DP.printcheck("spos=", spos);
		rv1=~A_mg*rv1;
		rv2=~A_mg*rv2;
		rv3=~A_mg*rv3;


		rv2.y = rv1.y;
		rv3.y = rv1.y;


  		rv1=A_mg*rv1;
		rv2=A_mg*rv2;
		rv3=A_mg*rv3;

	

		bd->SetMatrix_Matrix(nullptr, bh->GetMg());
		bd->DrawLine(G_P2, rv2, 0);
		bd->DrawLine(G_P1, rv2, 0);


		Vector v1=rv2-rv1;
		Vector v2=rv3-rv1;

		Int32 flag=0;
		Float rad=DP.Vector2AngleLocal(v2, v1, A_mg, flag);


		if(flagRB != flag)
		{
			if(flagRB==3 && flag ==0) flagR=1;
			else if(flagRB==0 && flag ==3) flagR=-1;

		}
		flagRB=flag;


        
		if(flagR == 1)
		{
			if(flag== 2 ||flag== 3 )
			{
				Float a=Deg(rad);
				rad=180+(180-abs(a));
                rad=Rad(rad);
			}
		}
		else if(flagR == -1)
		{
			if(flag== 1 ||flag== 0 )
			{
				Float a=Deg(rad);
				rad=-180-(180-abs(a));
                rad=Rad(rad);
			}
		}
		rad *= -1;
		Matrix cmg=A_mg;
		cmg.off=Vector(0.0, 0.0, 0.0);

		_RADROT=rad;
		Vector a1=~cmg*v1;
		////角度表示
		String Dpos="R="+String::FloatToString(Deg(rad));
		
	//	GePrint(Dpos);
		DP.DrawTextA(Dpos,G_markpoint,HUDOffset, bd, -150);

		//基準角度
		Float result2;

		result2= atan2 (a1.z,a1.x);
		//描画オブジェクト

		BaseContainer bc;
		bc.SetInt32(PRIM_DISC_SEG,18);
		bc.SetInt32(PRIM_DISC_CSUB,1);
		
		bc.SetFloat(PRIM_DISC_ORAD,Len(G_P2-G_P1));
		bc.SetBool(PRIM_SLICE,true);
		bc.SetFloat(PRIM_SLICEFROM, 0);
		
		Matrix mg;
		bc.SetInt32(PRIM_AXIS,2);
		Float vr=rad;
		Float vvr=result2;
	
		mg=MatrixRotY(vvr);

		bc.SetFloat(PRIM_SLICETO, vr);


		mg=cmg*mg;
		mg.off=G_P1;

		BaseObject *o=nullptr;
		o=GeneratePrimitive(nullptr, Odisc, bc, 1.0, false, nullptr);
		o->SetMg(mg);

		if(bd->DrawPolygonObject(bh,o,  DRAWOBJECT_XRAY_ON |DRAWOBJECT_NO_EOGL |DRAWOBJECT_USE_CUSTOM_COLOR | DRAWOBJECT_NOBACKCULL| DRAWOBJECT_FORCEBASE, nullptr, Vector(0, 1,0)) == DRAWRESULT_ERROR) return TOOLDRAW_0;
		BaseObject::Free(o);

		Matrix mr; //回転値のマトリックス
		mr=MatrixRotY(rad);
		
		if(!AltKey) data.SetFloat( ID_ROT_X, rad);

		//現在のモードを取得
		Int32 mode = doc->GetMode();
		if( mode== Mpoints || mode == Medges || mode == Mpolygons )
		{
			if(data.GetInt32(MDATA_SELECTMODE) == CH_EXTUBE  && ( mode == Medges || mode == Mpolygons) )
			{
				if(ObjCnt)
				{
					for(Int32 i=0; i<ObjCnt; i++)
					{
						BaseObject *op=mobj[i].op;
						if(!op) goto Error;
						if(!SetDrawExtubeAngle(doc,data, bd, bh, i, mr, rad )) goto Error;
					}
				}
			}
			else
			{
				if(ObjCnt)
				{
				
					for(Int32 i=0; i<ObjCnt; i++)
					{
						BaseObject *op=mobj[i].op;
						if(!op) goto Error;
						if(!SetPointAngle(op, i, mr )) goto Error;
					}
				}
			}
		}
		else if(mode == Mobject || mode == Mmodel )
		{
			if(ObjCnt)
			{
				for(Int32 i=0; i<ObjCnt; i++)
				{
					BaseObject *op=mobj[i].op;
					if(!op) goto Error;

					Vector p1=mobj[i].Mmg.off;
					Vector p2=G_P1;
					Vector p3=p1-p2;
					p3=~cmg*p3;
					p3=mr*p3;
					p3=cmg*p3;
					p3+=p2;

					Matrix mv=mobj[i].Mmg;
					mv.off=Vector(0,0,0);
					mv=~cmg*mv;
					mv=mr*mv;
					mv=cmg*mv;
					mv.off=p3;

					op->SetMg(mv);
				}
			}
		}

	}
	
	if(PlaneSetCnt==1)
	{
		bd->SetMatrix_Matrix(nullptr, bh->GetMg());
		bd->SetPen(Vector(0.0,1.0,1.0));
		bd->DrawHandle(PlV[0],DRAWHANDLE_BIG,0);
		bd->SetPen(Vector(0.0,0.0,1.0));
		bd->DrawLine(PlV[0],G_markpoint, 0);
	}
	else if(PlaneSetCnt==2)
	{
		bd->SetMatrix_Matrix(nullptr, bh->GetMg());
		bd->SetPen(Vector(0.0,1.0,1.0));
		bd->DrawHandle(PlV[0],DRAWHANDLE_BIG,0);
		bd->DrawHandle(PlV[1],DRAWHANDLE_BIG,0);
		bd->SetPen(Vector(0.0,0.0,1.0));
		bd->DrawLine(PlV[0],PlV[1], 0);
		bd->DrawLine(PlV[0],G_markpoint, 0);
	}

	//エッジ２センターモード時の表示設定


	if(Edge2Line_Step == 1)
	{
	//	GePrint("flag="+String::IntToString(G_SnapFlag));
		if( G_SnapFlag == 8 )
		{
			bd->SetMatrix_Matrix(nullptr, bh->GetMg());
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.5));
			bd->LineZOffset(4);
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->SetPen(Vector(1,0.92,0.2));
			bd->DrawLine(EdgeLineVectorA[0],EdgeLineVectorA[1], 0);
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
		//	DrawText( "A1", EdgeA.Vpoint[0], 10.0, bd, bh);
		}
	}
	else if(Edge2Line_Step == 2)
	{
	  	if( G_SnapFlag == 8 )
		{
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.5));
			DTOOL dp;
			bd->LineZOffset(4);
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->SetPen(Vector(1,0.92,0.2));
			bd->DrawLine(EdgeLineVectorA[0],EdgeLineVectorA[1], 0);
			bd->DrawLine(EdgeLineVectorB[0],EdgeLineVectorB[1], 0);
			Vector crossA, crossB;
			if (dp.CrossSectionB(EdgeLineVectorA[0], EdgeLineVectorA[1], EdgeLineVectorB[0], EdgeLineVectorB[1], &crossA, &crossB, data, EndMg))
			{
				CircleCenter =(crossA+crossB)/2;
				bd->SetMatrix_Matrix(NULL, bh->GetMg());
				bd->SetPen(Vector(1,0.92,0.2));
				bd->DrawLine(crossA,EdgeLineVectorA[1], 0);
				bd->DrawLine(crossB,EdgeLineVectorB[1], 0);

				bd->SetPen(Vector(1.0,0.0,0.5));
				bd->DrawHandle(CircleCenter,DRAWHANDLE_BIG,0);
			}
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
		}
		else
		{
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.5));
   			bd->LineZOffset(4);
			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->SetPen(Vector(1,0.92,0.2));
			bd->DrawLine(EdgeLineVectorA[0],EdgeLineVectorA[1], 0);
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
		}

	}



	return TOOLDRAW_HANDLES|TOOLDRAW_AXIS;
Error:
	 ResetTool();
	 
	return TOOLDRAW_0;
}
  
//--------------------------------------------------------------------------------------------------------------------------

//マウスカーソル
Bool SnapRot::GetCursorInfo(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, Float x, Float y, BaseContainer& bc)
{
	
	if( bc.GetId() == BFM_CURSORINFO_REMOVE) return true;
	bc.SetString( RESULT_BUBBLEHELP, GeLoadString( IDS_SnapSR_TOOL_HLP));



	//Ctrlキー取得
	BaseContainer state;
	GetInputState(BFM_INPUT_KEYBOARD, BFM_INPUT_QUALIFIER, state);
	Int32 qual = state.GetInt32(BFM_INPUT_QUALIFIER);
	Bool KAlt=false;
	CtrlKey=false;
	AltKey=false;
	PShift=false;
	switch(qual)
	{
	case QCTRL:
		CtrlKey=true;
		break;
	case QALT:
		KAlt=true;
		AltKey=true;
		break;
	case QSHIFT:
		PShift=true;
		break;
	}


	if (!_snap)
	{
		_snap = SnapCore::Alloc();
		if (!_snap) return false;
	}

	if (!_snap->Init(doc, bd))
		return false;	
	DTOOL DP;


	if (G_STEP == 0)
	{
		//作業平面設定と基準マトリックスの取得
		EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg, 1);
	}
	//Altキーを押していない場合
	if(!KAlt)
	{

		G_SnapFlag = 0;
		if(Edge2Line_Step != 0)
		{
			if(Edge2Line_Step == 1)	{
				G_markpoint = DP.SnapEdgeVector(doc, data, bd, x, y, Vector(), EndMg, &G_SnapFlag, G_STEP, G_P1, G_P2, &G_nvec, &FaceMg, &EdgeLineVectorA[0], &EdgeLineVectorA[1]);
			}
			else if(Edge2Line_Step == 2)	{
				G_markpoint = DP.SnapEdgeVector(doc, data, bd, x, y, Vector(), EndMg, &G_SnapFlag, G_STEP, G_P1, G_P2, &G_nvec, &FaceMg, &EdgeLineVectorB[0], &EdgeLineVectorB[1]);
			} 
		}
		else
		{
			
	

			//DP.printcheckR(" GetCorcor Rot=", r);
			if(G_STEP==0)
			{
				G_markpoint = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_markpoint, G_P1, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
					
			}
			else if(G_STEP==1)
			{
				G_P2 = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_P1, G_P2, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);

				G_markpoint=G_P2;
				if (G_SnapFlag == 0)
				{
					G_P2 = DP.GetFlatVectorPOS(doc, bd, data, x, y, EndMg, G_STEP, G_markpoint, G_P1, PlaneSet_Flag);
			
				}
				else
				{
					Vector p1 = ~EndMg*G_P1;
					Vector p2 = ~EndMg*G_markpoint;
					p2.y = p1.y;

					G_P2 = EndMg*p2;
					
				}
				//
				//DP.printcheck("    G_markpoint = ", G_markpoint);
			
				//Shiftキーで直行スナップ
				if(PShift)
				{
					G_markpoint = DP.GetDrawVec(doc, data, G_P1, G_markpoint, EndMg);
				}
				else if(CtrlKey)
				{
					Float Ang = data.GetFloat(ROT_ANGLE_VALUE);//読込みデータはラジアン
					G_markpoint = DP.ConstAngleFase(doc, data, G_P1, G_markpoint, Ang, EndMg);
				}
			}
			else if(G_STEP==2)
			{
				G_P3 = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_P2, G_P3, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
				//G_markpoint=DP.SnapMouseVector( doc, data, bd, x, y, EndMg.off, &EndMg, &G_SnapFlag,G_STEP, &G_P2, &G_P3, &G_nvec,  false, &FaceMg , &Guide, &_Starget, &SnapID  );
				G_markpoint = G_P3;
				if (G_SnapFlag == 0)
				{
					G_markpoint = DP.GetFlatVectorPOS(doc, bd, data, x, y, EndMg, G_STEP, G_markpoint, G_P1, PlaneSet_Flag);
				}
				else
				{
					Vector p1 = ~EndMg*G_P2;
					Vector p2 = ~EndMg*G_markpoint;
					p2.y = p1.y;
					G_P3 = EndMg*p2;
				}

				if(PShift)
				{
					G_markpoint = DP.GetDrawVec(doc, data, G_P1, G_markpoint, EndMg);
				}
				else if(CtrlKey)
				{
					//BaseObject *tObj=doc->GetActiveObject();
					Float Ang = data.GetFloat(ROT_ANGLE_VALUE);//読込みデータはラジアン
					if(data.GetInt32(MDATA_LIST ) == CH_WORLD)
					{
						G_markpoint = DP.ConstAngleFase(doc, data, G_P1, G_markpoint, Ang, EndMg);
					}
					else
					{
						G_markpoint = DP.LockAngle(doc, data, G_markpoint, Ang, G_P1, G_P2, EndMg);
					}
				}
			}
		}
	}

	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);

	
	return true;
}

//-------------------------------------------------------------------------------------
//メッセージ
Bool SnapRot::Message(BaseDocument* doc, BaseContainer& data, Int32 type, void* t_data)
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
		case MSG_DESCRIPTION_VALIDATE:
			{
				DTOOL DP;
				DP.SetGridLine(doc, data);
				if(!InteractiveValue(doc,data)) return false;
			}
			break;
	case MSG_DESCRIPTION_USERINTERACTION_END:
		{

			if(data.GetInt32(MDATA_SELECTMODE) ==  CH_EXTUBE)
			{
				if(doc->GetMode() == Medges || 	doc->GetMode() == Mpolygons)
				{
				}
				else
				{
					if( QuestionDialog(	"It is not a polygon mode or edge mode.\nAre you sure you want to change the polygon mode?"))
					{
						  doc->SetMode(Mpolygons);

					}
					else
					{
						data.SetInt32(	MDATA_SELECTMODE,CH_ROT); 
					}

				}
			}
		}
		break;

	case MSG_DESCRIPTION_POSTSETPARAMETER:
		{
			if (G_STEP == 0)
			{
				
				//ツールの平面タイプを変更した場合に即座に反映
				BaseDraw* bd = doc->GetActiveBaseDraw();
				DTOOL DP;
				EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg,1);//　基準平面を設定し最終マトリックスを返す
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
					Set3pointPlane(doc,data);
				break;
			case MDATA_RESET:
					ResetPlaneAxis(doc, data);
				break;
			case IDB_BTN_SET:
				ResetTool();
				EventAdd();
				break;
			case MDATA_SAVES:
					Priinit( data, true );
				break;
			case MDATA_CROSSPOSBTN:
				Edge2Line_Step=1;
 				EnableSnap(true, doc);
				EnableSnap(true, doc, SNAPMODE_EDGE);
			//	EnableSnap(true, doc, SNAPMODE_POINT);

				data.SetBool(SNAP_ENABLEG , true);
				data.SetBool(SNAP_EDGEG , true);
				//data.SetBool(SNAP_POINTG , true);
				EventAdd();

				break;
			case MDATA_CROSSRESETBTN:
				Edge2Line_Step=0;

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


//-------------------------------------------------------------------------------------

Vector SnapRot::ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection)
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

//////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------
////アクティブビューを取得し設定を変更
//Bool SnapRot::SetDPlane( BaseDocument *doc,BaseDraw* bd, BaseContainer &data,Bool flag, Int32 num)
//{
//	//現在のビューを取得
//	Int32 view =	bd->GetProjection();
//	if(view==6 || view==7) //top&bottom
//	{
//		data.SetInt32(IDE_Snap_AXIS,IDE_XZ);
//		BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		Matrix mg;
//		plane->SetMg(mg);
//		EndMg=mg;
//	}
//	else if(view==4 || view==5) //front &back
//	{
//		data.SetInt32(IDE_Snap_AXIS,IDE_XY);
//		BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		Matrix mg ;
//
//		 mg =MatrixRotX(Rad(-90.0));
//		 plane->SetMg(mg);
//		EndMg=mg;
//	}
//	else if(view==3 || view==2) //right&left
//	{
//		data.SetInt32(IDE_Snap_AXIS,IDE_YZ);
//		BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		Matrix mg =MatrixRotZ(Rad(-90.0));
//		plane->SetMg(mg);
//		EndMg=mg;
//	}
// 	else
//	{
//		DTOOL DP;
//	   DP.SetObjectPlane(GetActiveDocument(),data,flag);
//	   data.SetInt32(IDE_Snap_AXIS,num);
//
//	}
//	return true;
//}

Bool  SnapRot::GetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags)
{
	switch (id[0].id)
	{

	case MDATA_SAVES:

		flags |= DESCFLAGS_GET_PARAM_GET;
		break;
	}
	return SUPER::GetDParameter(doc, data, id, t_data, flags);
}




//////////////////////////////////////////////////////////////////////
Bool SnapRot::SetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags)
{
	switch(flags)
	{
	case DESCFLAGS_SET_USERINTERACTION:
			switch(id[0].id)
			{
				case MDATA_DRAG_IDE_OBJ_TARGET:
				{
					BaseObject *tObj=data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc);
					if(tObj)
					{
						ResetPlaneAxis(doc, data);
						getFlag=false;
					}
				}
				break;
				case MDATA_SAVES:
				{
					Priinit(data, true);
				}
					break;
			}
		break;



	}

	return SUPER::SetDParameter(doc, data, id, t_data, flags);
}


Bool SnapRot::RectPointALLVector(BaseObject *op, Int32 i )
{
	if(!op) return false;

	PointObject *PO=nullptr;

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
	if(!PO) return false;
	mobj[i].Mmg=op->GetMg();
	Int32 pcnt = PO->GetPointCount();
	if(!pcnt) return false;
	const Vector *vp=PO->GetPointR();
	if(pcnt)
	{
		mobj[i].scnt=pcnt;
		//mobj[i].vv=(Vector *) GeAlloc(pcnt*sizeof(Vector));
	   mobj[i].vv=NewMemClear(Vector,pcnt);
		
		if(!mobj[i].vv)
		{
		//	DeleteMem(mobj[i].vv);
			return false;
		}
		//ﾎﾟｲﾝﾄを格納
		for(Int32 j=0; j<pcnt; j++)
		{
			mobj[i].vv[j]=vp[j];
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////
//選択ポイント座標を記憶
//////////////////////////////////////////////////////////////////////
Bool SnapRot::RectPointVector(BaseObject *op, Int32 i )
{

	if(!op) return false;

	PointObject *PO=nullptr;

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
	if(!PO) return false;
	mobj[i].Mmg=op->GetMg();
	BaseSelect *bs=PO->GetPointS();
	Int32 pcnt = bs->GetCount();
	if(!pcnt) return false;
	Vector *vp=PO->GetPointW();

	if(pcnt)
	{
		mobj[i].scnt=pcnt;
	//	mobj[i].vv=(Vector *) GeAlloc(pcnt*sizeof(Vector));

		mobj[i].vv=NewMemClear(Vector,pcnt);

		if(!mobj[i].vv)
		{
		//	DeleteMem(mobj[i].vv);
			return false;
		}
		//mobj[i].pnum=(Int32 *) GeAlloc(pcnt*sizeof(Int32));
		mobj[i].pnum=NewMemClear(Int32,pcnt);

		if(!mobj[i].pnum)
		{
			DeleteMem(mobj[i].vv);
		//	DeleteMem(mobj[i].pnum);
			return false;
		}
		  
		Int32 ndx, seg = 0, smin, smax, i_cnt=0;
		//選択ﾎﾟｲﾝﾄを格納
		while( bs->GetRange(seg++,PO->GetPointCount(),&smin,&smax) )
		{
			for( ndx=smin; ndx<=smax; ndx++ )
			{
				mobj[i].vv[i_cnt]=vp[ndx];
				mobj[i].pnum[i_cnt]=ndx;
				i_cnt += 1;
			}
		}
	}

	if(op->GetType() == Opolygon)
	{
		if (op->GetDocument()->GetMode() == Mpolygons)
		{
			if (!GetSELPoly(op, i, 0))
			{
				DeleteMem(mobj[i].vv);
				DeleteMem(mobj[i].pnum);

				return false;
			}
		}
		if (op->GetDocument()->GetMode() == Medges)
		{
			if (!GetSELEdge(op, i, 0))
			{
				DeleteMem(mobj[i].vv);
				DeleteMem(mobj[i].pnum);

				return false;
			}
		}
	}

	return true;
}


//**************************************************
//選択ポイントを回転
//**************************************************
Bool SnapRot::SetPointAngle( BaseObject *op, Int32 i, Matrix mr )
{
  if(!op) return false;
    PointObject *point=nullptr;
    op=mobj[i].op;
    if(op->GetType() != Opolygon )
    {
        point=(PointObject*)op->GetCache();
        if(op->GetType() == Ospline)
        {
            point=ToSpline(op);
        }
        if(!point)
        {
            point=ToPoint(op);
        }
    }
    else
    {
        point=ToPoint(op);
    }

    if(!point) return false;
    //Vector *vp=point->GetPointW();
    //if(!vp) return false;
    //GePrint("sss");
    DTOOL dp;
    Int32 Scnt=mobj[i].scnt;
   
    if(Scnt)
    {
        if(op->GetType() == Ospline || op->GetType() == Offd )
        {
            Vector* vv=point->GetPointW();
            for(Int32 j=0; j<Scnt; j++)
            {
				Matrix cmg = EndMg;
                cmg.off=Vector(0.0,0.0,0.0);
                Vector v=mobj[i].vv[j]; //ポイント座標取得
                Matrix mmg1=op->GetMg(); //グローバルマトリックス取得
                v=mmg1*v;//ポイント座標をグローバル座標に変換
                v=~cmg*v;
                Vector p2=G_P1;
                p2=~cmg*p2;        //角度リセット
                Vector p3=v-p2;            //グローバルポイント座標から基点座標を引いてマトリックスoffに入れる
                p3=mr*p3;
                p3+=p2;
                p3=cmg*p3;
                p3=~mmg1*p3;
                vv[mobj[i].pnum[j]]=p3;
            }
            op->Message(MSG_UPDATE);
        }

        else if(op->GetType() == Opolygon )
        {

            AutoAlloc<Modeling> mod;
            if (!mod || !mod->InitObject(op)) return false;

            for(Int32 j=0; j<Scnt; j++)
            {
				Matrix cmg = EndMg;
                cmg.off=Vector(0.0,0.0,0.0);
                Vector v=mobj[i].vv[j]; //ポイント座標取得

                Matrix mmg1=op->GetMg(); //グローバルマトリックス取得
                v=mmg1*v;//ポイント座標をグローバル座標に変換
                v=~cmg*v;
                Vector p2=G_P1;
                p2=~cmg*p2;        //角度リセット
                Vector p3=v-p2;            //グローバルポイント座標から基点座標を引いてマトリックスoffに入れる
                p3=mr*p3;
                p3+=p2;
                p3=cmg*p3;
                p3=~mmg1*p3;
       

                if(!mod->SetPoint(op,mobj[i].pnum[j],p3, MODELING_SETPOINT_FLAG_EMPTY)) return false;
            //    vp[mobj[i].pnum[j]]=p3;
            }
            if (!mod->Commit(op, MODELING_COMMIT_UPDATE, nullptr )) return false;
        }
        //op->Message(MSG_UPDATE);
    }   

    return true;
}




//////////////////////////////////////////////////////////////////////
//ポリゴンのポイント座標を記憶
//////////////////////////////////////////////////////////////////////
Bool SnapRot::RectPolygonVector(BaseObject *op, Int32 i )
{
	if(!op) return false;

	PointObject *point=ToPoint(op);
	if(!point) return false;
	PolygonObject *Poly=ToPoly(op);
	if(!Poly) return false;
	BaseSelect *PolySel = Poly->GetPolygonS();	//選択ポリゴン
	Int32 SelCnt=PolySel->GetCount();			//選択ポリゴン数
	if(!SelCnt) return false;
	const Vector *vpoint=point->GetPointR();
	const CPolygon *Cpoly=Poly->GetPolygonR();

	if(SelCnt)
	{
		mobj[i].Fcnt=SelCnt;
		mobj[i].scnt=SelCnt*4;
	//	mobj[i].vv=(Vector *) GeAlloc(SelCnt*4*sizeof(Vector));

		mobj[i].vv=NewMemClear(Vector, SelCnt*4);


		if(!mobj[i].vv)
		{
		//	DeleteMem(mobj[i].vv);
			return false;
		}
		//mobj[i].pnum=(Int32 *) GeAlloc(SelCnt*4*sizeof(Int32));
		mobj[i].pnum=NewMemClear(Int32, SelCnt*4);

		if(!mobj[i].pnum)
		{
			DeleteMem(mobj[i].vv);
			return false;
		}
		//mobj[i].Snum=(Int32 *) GeAlloc(SelCnt*sizeof(Int32));

		mobj[i].Snum=NewMemClear(Int32, SelCnt);

		if(!mobj[i].Snum)
		{
			DeleteMem(mobj[i].vv);
			DeleteMem(mobj[i].pnum);
	//		DeleteMem(mobj[i].Snum);
			return false;
		}

		//GePrint("SelCnt="+String::IntToString(	SelCnt));
			
	//	Int32 ndx, seg = 0, smin, smax, i_cnt=0, pa=0;
		Int32  i_cnt=0, pa=0;
		//選択ﾎﾟｲﾝﾄを格納
	//	while( PolySel->GetRange(seg++,Poly->GetPolygonCount(), &smin,&smax) )
	//	{
		//	for( ndx=smin; ndx<=smax; ndx++ )
		//	{
		for(Int32 ndx=0; ndx< Poly->GetPolygonCount(); ndx++)
		{
			if(	 PolySel->IsSelected(ndx))
			{
				Vector P[4];
				P[0]=vpoint[Cpoly[ndx].a];
				P[1]=vpoint[Cpoly[ndx].b];
				P[2]=vpoint[Cpoly[ndx].c];
				P[3]=vpoint[Cpoly[ndx].d];
                
				Int32 num[4];
				num[0]=Cpoly[ndx].a;
				num[1]=Cpoly[ndx].b;
				num[2]=Cpoly[ndx].c;
				num[3]=Cpoly[ndx].d;

				mobj[i].Snum[pa]=ndx;
				pa +=1;

				for(Int32 j=0; j<4; j++)
				{
					mobj[i].vv[i_cnt]=P[j];
					mobj[i].pnum[i_cnt]=num[j];
					i_cnt += 1;
				}
			}
		}
		   
		///////////////////////////////////////
		//エッジを調べる
		if(!GetSELEdge(op, i, 2))
		{
			DeleteMem(mobj[i].vv);
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].Snum);
			return false;
		}
		///////////////////////////////////////
		
	}
	return true;
}





//////////////////////////////////////////////////////////////////////
//エッジのポイント座標を記憶
//////////////////////////////////////////////////////////////////////
Bool SnapRot::RectEdgeVector(BaseObject *op, Int32 i )
{
	if(!op) return false;
	
	PointObject *point=ToPoint(op);
	if(!point) return false;
	const Vector *vpoint=point->GetPointR();

	PolygonObject *Pobj=ToPoly(op);	//ポリゴン変換
	if(!Pobj) return false;
	
	const CPolygon*  poly = Pobj->GetPolygonR(); //ポリゴンリスト取得

	BaseSelect *Esel=Pobj->GetEdgeS();

	//選択エッジの数を取得
	Int32 ecnt=Esel->GetCount();
	if(!ecnt) return false;
	mobj[i].Ecnt=ecnt;


	//mobj[i].Enum=(Int32 *) GeAlloc(ecnt*sizeof(Int32));

	mobj[i].Enum= NewMemClear(Int32, ecnt);

	if(!mobj[i].Enum)
	{
	//	DeleteMem(mobj[i].Enum);
		return false;
	}
	
	//Int32 *se=(Int32 *) GeAlloc(ecnt*2*sizeof(Int32));

	Int32 *se= NewMemClear(Int32, ecnt*2);

	if(!se)
	{
	//	DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}

	//Int32 *se2=(Int32 *) GeAlloc(ecnt*2*sizeof(Int32));

	Int32 *se2= NewMemClear(Int32, ecnt*2);


	if(!se2)
	{
	//	DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}
	Int32 ndx, seg = 0, smin, smax, i_cnt=0, ocnt=0, ii=0;
	while( Esel->GetRange(seg++,Pobj->GetPolygonCount()*4, &smin,&smax) )
	{
		for( ndx=smin; ndx<=smax; ndx++ )
		{

			ocnt +=1;
			Int32 polyNdx;
			polyNdx = ndx >> 2; //ポリゴン番号
			mobj[i].Enum[ii]=ndx;
			//GePrint("ndx="+String::IntToString(ndx));


			ii += 1;
			//GePrint("Esel="+Int32ToString(polyNdx));
			switch( ndx % 4 ) //エッジ番号
			{
			case 0:
				se[i_cnt]=poly[polyNdx].a;
				se[i_cnt+1]=poly[polyNdx].b;
				i_cnt +=2;
				break;
			case 1:
				se[i_cnt]=poly[polyNdx].b;
				se[i_cnt+1]=poly[polyNdx].c;
				i_cnt +=2;
				break;
			case 2:
				se[i_cnt]=poly[polyNdx].c;
				se[i_cnt+1]=poly[polyNdx].d;
				i_cnt +=2;
				break;
			case 3:
				se[i_cnt]=poly[polyNdx].d;
				se[i_cnt+1]=poly[polyNdx].a;
				i_cnt +=2;
				break;
			}
		}
	}


	//UVポリマップ
	UChar* map = nullptr;

	map= NewMemClear(UChar, i_cnt);


	if(!map)
	{
	//	DeleteMem(map);
		DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}

	for(Int32 j=0; j<i_cnt; j++)
	{
		if(!map[j])
		{
			for(Int32 k=j+1; k<i_cnt; k++)
			{
				if(se[j]== se[k])
				{
					map[j]=true;
				}
			}
		}
	}

	Int32 tcnt=0;
	for(Int32 j=0; j<i_cnt; j++)
	{
		if(!map[j])
		{
			tcnt+=1;
		}
	}

	mobj[i].scnt=tcnt;
	mobj[i].vv = NewMemClear(Vector, tcnt);

	if(!mobj[i].vv)
	{
	//	DeleteMem(mobj[i].vv);
		DeleteMem(map);
		DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}

	//mobj[i].pnum=(Int32 *) GeAlloc(tcnt*sizeof(Int32));

	mobj[i].pnum = NewMemClear(Int32, tcnt);

	if(!mobj[i].pnum)
	{
	//	DeleteMem(mobj[i].pnum);
		DeleteMem(mobj[i].vv);
		DeleteMem(map);
		DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}

	Int32 c=0;
	for(Int32 j=0; j<i_cnt; j++)
	{
		if(!map[j])
		{
			mobj[i].vv[c]=vpoint[se[j]];
			mobj[i].pnum[c]=se[j];
			c+=1;
		}
	}

	if(!GetSELPoly(op, i, 1))
	{
		DeleteMem(mobj[i].pnum);
		DeleteMem(mobj[i].vv);
		DeleteMem(map);
		DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}


	mobj[i].emap= NewMemClear(UChar, mobj[i].Ecnt);
	if(! mobj[i].emap) 
	{
		DeleteMem(mobj[i].pnum);
		DeleteMem(mobj[i].vv);
		DeleteMem(map);
		DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;

	}

	Modeling* mkernel=Modeling::Alloc();
	if(!mkernel) return false;
	if (!mkernel->InitObject(op)) return false;

	 for(Int32 j=0; j<mobj[i].Ecnt-1; j++)
	 {
		//エッジ番号
		Int32 ep=  mobj[i].Enum[j];
		for(Int32 k=j+1; k<mobj[i].Ecnt; k++)
		{
			if( !mobj[i].emap[k])
			{
				Int32 a, b, c, d;
                
				if(mkernel->GetOriginalEdgePoints(op,ep,a,b))
				{
					//エッジ番号
					Int32 ep2=  mobj[i].Enum[k];
					if(mkernel->GetOriginalEdgePoints(op,ep2,c,d))
					{
					   if( a==c || a==d)
					   {
						   if( b== c || b == d)
						   {
							   mobj[i].emap[k]=true;
						   }
					   }
					}
				}
			}
		}
	 }

	mobj[i].SEcnt=0;
	 for(Int32 j=0; j<mobj[i].Ecnt; j++)
	 {
		 if( !mobj[i].emap[j])
		 {
			 mobj[i].SEcnt++;
		 }
	 }
	DeleteMem(map);
	return true;
}

/////////////////////////////////////////////////////////////////
//アンドォ設定
//////////////////////////////////////////////////////////////////////
Bool SnapRot::AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE type)
{
	Int32 a;
	for (a = 0; a < arr->GetCount(); a++)
	{
		if (!doc->AddUndo(type, arr->GetIndex(a)))
			return false;
	}
	return true;
}

/////////////////////////////////////////////
Bool SnapRot::SetAngle(BaseDocument* doc, BaseContainer& data)
{

	//値読み込み
	Float r=data.GetFloat(ID_ROT_X);

	Matrix mr; //回転値のマトリックス
	mr=MatrixRotY(r);

	////選択オブジェクトの取得
	AutoAlloc <AtomArray> arr;
	if (!doc || !arr) return false;

	//現在のモードを取得
	Int32 mode = doc->GetMode();
	if( mode== Mpoints || mode == Medges || mode == Mpolygons )
	{
		doc->GetActivePolygonObjects(*arr, true);
		Int32 cnt=arr->GetCount();
		if(cnt)
		{
			for(Int32 i=0; i<cnt; i++)
			{
				BaseObject *op=(BaseObject*)arr->GetIndex(i);
				if(!op) goto Error;

                if(!SetPointAngle(op, i, mr )) goto Error;
			}
		}
	}
	else if(mode == Mobject || mode == Mmodel )
	{
		doc->GetActiveObjects(*arr, GETACTIVEOBJECTFLAGS_CHILDREN);
		Int32 cnt=arr->GetCount();
		
		if(cnt)
		{
			Matrix cmg=EndMg;
			cmg.off=Vector(0.0, 0.0, 0.0);
			for(Int32 i=0; i<cnt; i++)
			{
				BaseObject *op=(BaseObject*)arr->GetIndex(i);
				if(!op) goto Error;

				Vector p1=mobj[i].Mmg.off;
				Vector p2=G_P1;
				Vector p3=p1-p2;
				p3=~cmg*p3;
				p3=mr*p3;
				p3=cmg*p3;
				p3+=p2;

				Matrix mv=mobj[i].Mmg;
				mv.off=Vector(0,0,0);
				mv=~cmg*mv;
				mv=mr*mv;
				mv=cmg*mv;
				mv.off=p3;
				op->SetMg(mv);
			}
		}
	}
	ResetTool();
	
//	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
	EventAdd();
	return true;


Error:
	 ResetTool();
	return false;
}

//*************************************************************************
//ポリゴンを調べる
Bool SnapRot::GetSELPoly(BaseObject *op, Int32 num, Int32 id)
{
	//選択の変換
		BaseContainer bc;
		ModelingCommandData cd;
		BaseDocument *doc=GetActiveDocument();
		cd.doc = doc;
		cd.op = mobj[num].op;
		bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, id);
		bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,2);
		cd.bc=&bc;
		cd.mode=MODELINGCOMMANDMODE_ALL;
		if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
		PolygonObject *po=ToPoly(mobj[num].op);
		//pcnt=po->GetPolygonCount();
		BaseSelect *Pbs=po->GetPolygonS();
		Int32 pcnt=Pbs->GetCount();
		
		if(pcnt)
		{
			mobj[num].Fcnt=pcnt;
			//mobj[num].Snum=(Int32 *) GeAlloc(pcnt*sizeof(Int32));

			mobj[num].Snum=NewMemClear( Int32, pcnt);

			if(!mobj[num].Snum)
			{
			//	DeleteMem(mobj[num].Snum);
				return false;
			}
			Int32 ndx, seg = 0, smin, smax, p=0;
			//選択ﾎﾟｲﾝﾄを格納
			while( Pbs->GetRange(seg++,po->GetPolygonCount(),&smin,&smax) )
			{
				for( ndx=smin; ndx<=smax; ndx++ )
				{
					mobj[num].Snum[p]=ndx;
					p +=1;
				}
			}
		}
		
		return true;
}


//////////////////////////////////////////////////////////////////////
//エッジを調べる
//////////////////////////////////////////////////////////////////////
Bool SnapRot::GetSELEdge(BaseObject *op, Int32 num, Int32 id)
{
		BaseDocument *doc=GetActiveDocument();
		if(!doc) return false;
		

		//CallCommand(1011179); // Outline Selection
		  
		//選択をエッジに変換
		BaseContainer bc;
		ModelingCommandData cd;															   
		cd.doc = doc;
		cd.op = mobj[num].op;
		bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, id);
		bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,1);

		cd.bc=&bc;
		cd.mode=MODELINGCOMMANDMODE_ALL;
		if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
		
		PolygonObject *po=ToPoly(mobj[num].op);

		
		//選択エッジを取得
		BaseSelect *Pbs=po->GetEdgeS();
		//エッジの数を取得
		Int32 pcnt=Pbs->GetCount();

		//エッジの重複処置
		UChar* emap0= NewMemClear(UChar, pcnt);
		if(!emap0) return false;


		Int32* SelEs= NewMemClear(Int32, pcnt);
		 if(!SelEs)
		 {
			DeleteMem(emap0); 			 
			return false;
		 }
		//GePrint("Pbs="+String::IntToString(pcnt));
		if(pcnt)
		{

			Int32 ndx, seg = 0, smin, smax, p=0;
			//選択エッジIDを取得
			while( Pbs->GetRange(seg++,po->GetPolygonCount()*4, &smin,&smax) )
			{
				for( ndx=smin; ndx<=smax; ndx++ )
				{
					//GePrint(" ndx="+String::IntToString(ndx));
					SelEs[p]=ndx;
					p +=1;
				}
			}


			//エッジの重複処理

			Modeling* mkernel=Modeling::Alloc();
			if(!mkernel)
			{
				DeleteMem(emap0); 
				DeleteMem(SelEs);
				return false;
			}
			if (!mkernel->InitObject(op))
   			{
				DeleteMem(emap0); 
				DeleteMem(SelEs);
				return false;
			}

			for(Int32 j=0; j<  pcnt-1; j++)
			{
				Int32 ep= SelEs[j];
				for(Int32 k=j+1; k<pcnt; k++)
				{
					if( !emap0[k])
					{
						Int32 a, b, c, d;
						if(mkernel->GetOriginalEdgePoints(op,ep,a,b))
						{
							//エッジ番号
							Int32 ep2=  SelEs[k];
							if(mkernel->GetOriginalEdgePoints(op,ep2,c,d))
							{
							   if( a==c || a==d)
							   {
								   if( b== c || b == d)
								   {
									   emap0[k]=true;
								   }
							   }
							}
						}
					}
				}
			}

			//GePrint("-------------------------");
			//for(Int32 j=0; j<  pcnt; j++)
			//{
			//	if( !emap0[j])
			//	{
			//	   Int32 ep2=  SelEs[j];
			//	   GePrint("ep2="+String::IntToString(ep2));
			//	}
			//}


			Neighbor nbr ;
  			PointObject* Pop=ToPoint(op);
					
			if(!nbr.Init(Pop->GetPointCount(), po->GetPolygonR(), po->GetPolygonCount(), NULL))
			{
				DeleteMem(emap0); 
				DeleteMem(SelEs);
				return false;
			}
		

			Int32 ca=0;
   			for(Int32 j=0; j<  pcnt; j++)
			{
 				if( !emap0[j])
				{
					Int32 a, b, pa, pb;
					pa=pb=-1;
					Int32 ep= SelEs[j];
					if(mkernel->GetOriginalEdgePoints(op,ep,a,b))
					{
						nbr.GetEdgePolys(a,b,&pa, &pb);
						Int32 cc=0;
						for(Int32 g=0; g< mobj[num].Fcnt; g++)
						{
							if( pa == mobj[num].Snum[g])  cc++;
							if( pb == mobj[num].Snum[g])  cc++;
								
							if(cc==2)
							{
								emap0[j] = true;
							}
							  
						}
					}
				}
			}


			//GePrint("-------------------------");
			for(Int32 j=0; j<  pcnt; j++)
			{
				if( !emap0[j])
				{
					ca++;
				}
			}

			if(ca >0)
			{
				
				//アウトラインエッジを取得
				mobj[num].Ecnt=ca; //選択エッジの本数
				//mobj[num].Enum=(Int32 *) GeAlloc(pcnt*sizeof(Int32));
				mobj[num].Enum=NewMemClear( Int32, ca);
				if(!mobj[num].Enum)
				{
					DeleteMem(emap0); 
					DeleteMem(SelEs);
					return false;
				}

				Pbs->DeselectAll();

				Int32 cnt=0;
   				for(Int32 j=0; j<  pcnt; j++)
				{
 					if( !emap0[j])
					{
						Int32 ep= SelEs[j];
						mobj[num].Enum[cnt]=ep;
						Pbs->Select(ep);
						cnt++;
					}
				}
			}
		}
 		DeleteMem(emap0); 
		DeleteMem(SelEs);
	
		return true;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
Bool SnapRot::InteractiveValue(BaseDocument *doc, BaseContainer &data)
{
	if(!doc) return false;
	if(G_STEP!=2) return false;
	Float r=data.GetFloat( ID_ROT_X);
	Vector v3=G_markpoint;
	Vector v2=G_P2;
	Vector v1=G_P1;

	Vector L1=v2-v1;
	Vector L2=v3-v1;
	Matrix Mg= EndMg;
	Mg.off=Vector();

	L1=~Mg*L1;
	L2=~Mg*L2;
	Vector L3=Vector(1,0,0);
	L1.Normalize();
	//Float Frad =VectorAngle(L1,L3);
	Float Frad =GetAngle(L1,L3);
	//Vector g=L1%L3; //外積を算出
	Vector g = Cross(L1,L3); //外積を算出
	if(g.y<0)
	{
		Frad *= -1;
	}
	Frad=Frad+r;
	Vector Vlen=Vector(Len(L2),0,0);
	Matrix RY=MatrixRotY(Frad);
	Vlen=RY*Vlen;
	Vlen=Mg*Vlen;
	G_markpoint=G_P1+Vlen;

	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
	return true;

}


//*********************************************************************
//3pointplane
//*********************************************************************
Bool SnapRot::Set3pointPlane(BaseDocument* doc, BaseContainer &data )
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



Bool SnapRot::GetDDescription(BaseDocument* doc, BaseContainer& data, Description* description, DESCFLAGS_DESC& flags)
{
	if (! description->LoadDescription("SnapRotate") ) return false;
	//DTOOL dp;
	//if(!dp.CheckSnapSetting( doc,data)) return false;
	BaseContainer *bc = description->GetParameterI(DescLevel(STtoolG), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(STop), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(ValueOFF), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(MDATA_BTN_VRESET), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(MDATA_BOOL_OFFSET), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(GROUP_P), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);

	bc=description->GetParameterI(DescLevel(SNAP_PRE), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(PRE_COLOR), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	bc=description->GetParameterI(DescLevel(MDATA_NORMALPOSITION), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);

	bc=description->GetParameterI(DescLevel(SNAP_SECTION), nullptr);
	if (bc) bc->SetBool(DESC_HIDE, true);
	//flags |= DESCFLAGS_DESC_LOADED|DESCFLAGS_DESC_RECURSIONLOCK;

	DescID did3 = DescLevel(MDATA_SAVES, DTYPE_NONE, 0);
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



void SnapRot::Priinit(BaseContainer& data, Bool flag )
{
	String str= "SnR.dat";
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
void SnapRot::ReadData(BaseContainer bc, BaseContainer& data)
{
	data.SetFloat(ROT_ANGLE_VALUE, bc.GetFloat(ROT_ANGLE_VALUE) );
	data.SetFloat(ID_ROT_X, bc.GetFloat(ID_ROT_X) );
	data.SetInt32(IDE_MODE, bc.GetInt32(IDE_MODE) );

	data.SetInt32( IDE_Snap_AXIS, bc.GetInt32( IDE_Snap_AXIS));
	data.SetBool(MDATA_BOOL_GRID_FLAG, bc.GetBool(MDATA_BOOL_GRID_FLAG) );
	data.SetFloat( MDATA_GRID_VALUE, bc.GetFloat(MDATA_GRID_VALUE)  );
	data.SetInt32( MDATA_GRIDLINE, bc.GetInt32( MDATA_GRIDLINE) );
	data.SetInt32( MDATA_LINE, bc.GetInt32(MDATA_LINE)  );
	data.SetInt32( MDATA_GRID_LIST, bc.GetInt32(MDATA_GRID_LIST) );
	data.SetInt32(  MDATA_LIST, bc.GetInt32( MDATA_LIST));
    data.SetInt32( MDATA_EXTUBESUBDEV , bc.GetInt32( MDATA_EXTUBESUBDEV ));
	data.SetInt32(  MDATA_SELECTMODE, bc.GetInt32( MDATA_SELECTMODE));
	data.SetBool( 	MDATA_SW2D, bc.GetBool( MDATA_SW2D));
	data.SetBool(MDATA_VISIBLE, bc.GetBool(MDATA_VISIBLE));
}

//デフォルトデータセット
void SnapRot::DefoultSet( BaseContainer& data)
{
	data.SetFloat(ROT_ANGLE_VALUE, Rad(5.0));
	data.SetFloat(ID_ROT_X, 0.0);
	data.SetInt32(IDE_MODE, CH_3SNAP);

	data.SetInt32( IDE_Snap_AXIS, IDE_XZ);
	data.SetBool(MDATA_BOOL_GRID_FLAG, false);
	data.SetFloat( MDATA_GRID_VALUE, 100 );
	data.SetInt32( MDATA_GRIDLINE, 100 );
	data.SetInt32( MDATA_LINE, 10 );
	data.SetInt32( MDATA_GRID_LIST, CH_GRID_L1);
	data.SetInt32( MDATA_LIST, CH_WORLD);
    data.SetInt32( MDATA_EXTUBESUBDEV , 10);
	data.SetInt32( MDATA_SELECTMODE , CH_ROT);
	data.SetBool( 	MDATA_SW2D, false);
	data.SetBool( MDATA_VISIBLE, false);
}


//パラメータ保存
void SnapRot::SaveData(HyperFile* hf, BaseContainer& data)
{
	BaseContainer bc;

	bc.SetFloat(ROT_ANGLE_VALUE, data.GetFloat(ROT_ANGLE_VALUE) );
	bc.SetFloat(ID_ROT_X, data.GetFloat(ID_ROT_X) );
	bc.SetInt32(IDE_MODE, data.GetInt32(IDE_MODE) );

	bc.SetInt32( IDE_Snap_AXIS, data.GetInt32( IDE_Snap_AXIS));
	bc.SetBool(MDATA_BOOL_GRID_FLAG, data.GetBool(MDATA_BOOL_GRID_FLAG) );
	bc.SetFloat( MDATA_GRID_VALUE, data.GetFloat(MDATA_GRID_VALUE)  );
	bc.SetInt32( MDATA_GRIDLINE, data.GetInt32( MDATA_GRIDLINE) );
	bc.SetInt32( MDATA_LINE, data.GetInt32(MDATA_LINE)  );
	bc.SetInt32( MDATA_GRID_LIST, data.GetInt32(MDATA_GRID_LIST) );
	bc.SetInt32(  MDATA_LIST, data.GetInt32( MDATA_LIST));
    bc.SetInt32( MDATA_EXTUBESUBDEV , data.GetInt32( MDATA_EXTUBESUBDEV  ));
	bc.SetInt32(  MDATA_SELECTMODE, data.GetInt32( MDATA_SELECTMODE));
	bc.SetBool( MDATA_SW2D, data.GetBool( MDATA_SW2D));
	bc.SetBool(MDATA_VISIBLE, data.GetBool( MDATA_VISIBLE));
	String ver=GeLoadString(IDC_VER);
	bc.SetString( 999,ver);
	hf->WriteContainer(bc);
}



//**************************************************
//
//**************************************************
Bool SnapRot::ExtubePoly(BaseDocument* doc, BaseContainer& data, Int32 i )
{
  	if(!doc) return false;
	
	BaseObject* op=mobj[i].op;
	if(!op) return false;


 	PointObject *point=nullptr;
	if(op->GetType() != Opolygon )
	{
		point=(PointObject*)op->GetCache();
		if(op->GetType() == Ospline)
		{
			point=ToSpline(op);
		}
		if(!point)
		{
			point=ToPoint(op);
		}
	}
	else
	{
		point=ToPoint(op);
	}

	if(!point) return false;

	DTOOL dp;
	Int32 Scnt=mobj[i].scnt;

 	if(Scnt)
	{

		Int32 Pcnt= point->GetPointCount(); //ポイント数
		PolygonObject* Ppoly=ToPoly(op);

 		//回転の分割数
		Int32 subdev =data.GetInt32( MDATA_EXTUBESUBDEV);
		Float r = _RADROT / (Float)( subdev+1);

		doc->AddUndo(UNDOTYPE_CHANGE, op);

		if(op->GetType() == Opolygon)
		{
			 
            Vector* vv=point->GetPointW();
            BaseSelect* Pbs= point->GetPointS();
            
             //  ポリゴンモード時に選択ポリゴンを回転
            if(doc->GetMode() ==Mpolygons)
            {

                //選択をエッジに変換
                BaseContainer bc0;
                ModelingCommandData cd0;
                cd0.doc = doc;
                cd0.op = mobj[i].op;

                bc0.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
                bc0.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,1);
                
                cd0.bc=&bc0;
                cd0.mode=MODELINGCOMMANDMODE_ALL;
                if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd0)) return false;          
                
                //選択ポリゴンの切り離し
                BaseContainer bc;
                bc.SetBool(MDATA_DISCONNECT_PRESERVEGROUPS, true);
                ModelingCommandData cd;
                cd.op = mobj[i].op;
                cd.bc=&bc;
                if(doc->GetMode() == Mpolygons)   cd.mode=MODELINGCOMMANDMODE_POLYGONSELECTION;
                if(!SendModelingCommand(MCOMMAND_DISCONNECT, cd)) return false;

                op->Message(MSG_UPDATE);
                  
                //選択ポイントを全非選択
                Pbs->DeselectAll();

                 //-----------------------------------------------------------------------
                //選択ポリゴンを選択ポイントに変換
                BaseContainer bc2;
                ModelingCommandData cd2;
                cd2.doc = doc;
                cd2.op = mobj[i].op;
                bc2.SetInt32(MDATA_CONVERTSELECTION_LEFT, 2);
                bc2.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,0);
                cd2.bc=&bc2;
                cd2.mode=MODELINGCOMMANDMODE_ALL;
                if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd2)) return false;
                 //-----------------------------------------------------------------------

                vv=point->GetPointW();
                //選択ポイント取得
                Pbs= point->GetPointS();
                
                for (Int32 id=0; id<point->GetPointCount(); id++)
                {
                    if (Pbs->IsSelected(id))
                    {
                        vv[id]= Rad2Vector( op, vv[id],G_P1, _RADROT, EndMg );
                    }
                }
                op->Message(MSG_UPDATE);
                
                
                BaseSelect* eebs=Ppoly->GetEdgeS();                
                Modeling* mkernel=Modeling::Alloc();
                if(!mkernel)  return false;
                if (!mkernel->InitObject(op))  return false;

                Neighbor nbr ;
                if(!nbr.Init(point->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL))
                {
                    return false;
                }

                for(Int32 j=0; j<  Ppoly->GetPolygonCount()*4; j++)
                {
                    if(eebs->IsSelected(j))
                    {
                        Int32 a, b;
                        Int32 ep = j;//エッジidを取得

                        Int32 pa,pb;
                        if(mkernel->GetOriginalEdgePoints(op,ep,a,b))//エッジidからポイントidを取得
						{
                            nbr.GetEdgePolys(a,b,&pa, &pb);
                            Int32 cc=0;
                            for(Int32 g=0; g< mobj[i].Fcnt; g++)
                            {
                                if( pa == mobj[i].Snum[g])  cc++;
                                if( pb == mobj[i].Snum[g])  cc++;
                            }
                            if(cc != 0)
                            {
                                eebs->Deselect(j);
                            }
                        } 
                    }
                }   
            }
			 
            
            //押し出す前のポリゴン数を取得
			Int32 PolyCnt=Ppoly->GetPolygonCount();
			//押し出す前のポイント数を取得
            Int32 TpcntA= point->GetPointCount();
            //選択エッジを取得
			BaseSelect* cbs= Ppoly->GetEdgeS();

            if(doc->GetMode() == Mpolygons)
            {
                //エッジからポリゴンidを取得し選択ポリゴンと同じ場合はエッジ選択から外す
                for (Int32 id=0; id<Ppoly->GetPolygonCount()*4; id++)
                {
                    if (cbs->IsSelected(id))
                    {
                        Int32 s=  id >> 2;	 //エッジからポリゴンIDを取得
                        for(Int32 c=0; c<  mobj[i].Fcnt; c++)
                        {
                            Int32 f= mobj[i].Snum[c];
                            if( s == f)
                            {
                                cbs->Deselect(id);	
                                break;
                            }
                        }
                    }
                }
            }

            //選択エッジの配列を用意
			Int32 cc=  cbs->GetCount();
            
            Int32* Eids=NewMemClear(Int32, cbs->GetCount());
			Int32 in=0;
  			for (Int32 id=0; id<Ppoly->GetPolygonCount()*4; id++)
			{
				if (cbs->IsSelected(id))
				{
					 Eids[in]=id;
					 in++;
				}
			}

            //------------------------------------------------------------------
			//ポリゴンを押し出し
 			BaseContainer bc3;
			ModelingCommandData cd3;
			bc3.SetFloat( MDATA_EXTRUDE_OFFSET, 0);
			bc3.SetBool( MDATA_EXTRUDE_PRESERVEGROUPS, true);
			bc3.SetInt32(MDATA_EXTRUDE_SUBDIVISION, subdev);
			bc3.SetBool(MDATA_EXTRUDE_CREATENGONS, false);
			bc3.SetFloat( MDATA_EXTRUDE_EDGEANGLE, 0.0);
			cd3.bc=&bc3;
			cd3.op = mobj[i].op;
			cd3.mode=MODELINGCOMMANDMODE_EDGESELECTION;
			 if(!SendModelingCommand(ID_MODELING_EXTRUDE_TOOL, cd3)) return false;
            //------------------------------------------------------------------


            //選択エッジを全て外す
            cbs->DeselectAll();
			// 
			Modeling* mkernel=Modeling::Alloc();
			if(!mkernel)return false;

			if (!mkernel->InitObject(op))return false;
          
			Neighbor nbr;
 			if(!nbr.Init(point->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;

			////ポリゴン情報を取得
            CPolygon* CP= Ppoly->GetPolygonW();
			////ポイント座標を取得
            vv=point->GetPointW();
  
			////押し出した後のポイント数を取得
            Int32 TpcntB =point->GetPointCount();

            UChar* map=NewMemClear(UChar,TpcntB);
            if(!map) return false;
            for(Int32 j=0; j< TpcntA; j++)
            {
                map[j]=true;
            }
     
   //         //押し出したポリゴンのポイントを修正
     		for (Int32 id=0; id<cc; id++)
			{
				Int32 ep= Eids[id]; //エッジid取得

				Int32 idd[2];     //ポイント番号
                Int32 pa, pb;   //ポリゴン番号
				pa=pb=-1;

   //             //エッジを構成するポイントidからポリゴンidを取得
   				if(mkernel->GetOriginalEdgePoints(op,ep,idd[0],idd[1]))
				{
					nbr.GetEdgePolys(idd[0],idd[1],&pa, &pb);
					Int32 pid;

					if(pa >= PolyCnt) pid=pa;
					else pid=pb;
                    
                    Int32 ee=-1;

					if((CP[pid].a == idd[0] && CP[pid].d == idd[1]) || (CP[pid].a == idd[1] && CP[pid].d == idd[0]))
					{
						ee=1;
						idd[0]=	CP[pid].b;
						idd[1]=	CP[pid].c;

					}
					else if((CP[pid].d == idd[0] && CP[pid].c == idd[1]) ||(CP[pid].d == idd[1] && CP[pid].c == idd[0]) )
					{
						ee=0;
						idd[0]=	CP[pid].a;
						idd[1]=	CP[pid].b;
					}
  					else if((CP[pid].c == idd[0] && CP[pid].b == idd[1]) ||(CP[pid].c == idd[1] && CP[pid].b == idd[0]) )
					{
						ee=3;
						idd[0]=	CP[pid].d;
						idd[1]=	CP[pid].a;
					}
					else if(( CP[pid].b == idd[0] && CP[pid].a == idd[1]) ||( CP[pid].b == idd[1] && CP[pid].a == idd[0]) )
					{
						ee=2;
						idd[0]=	CP[pid].b;
						idd[1]=	CP[pid].c;
					}

                    //接続するポリゴンidを取得
                    for( Int32 k=1; k < subdev + 2; k++)
					{
                        for(Int32 n=0; n<2; n++)
                        {
                            if(!map[idd[n]])
                            {
        					Float cr=(Float) r* k;
								vv[idd[n]]= Rad2Vector( op, vv[idd[n]],G_P1, cr, EndMg );
                                map[idd[n]]=true;
                            }
                        }
      
                        PolyInfo* pli= nbr.GetPolyInfo(pid);
                        pli->mark[ee];
                        //隣のポリゴンidを取得
                        Int32 na=pli->face[ee];
              
                        switch (ee)
   
                        {
                            case 0: idd[0]=CP[na].a; idd[1]=CP[na].b; break;
                            case 1: idd[0]=CP[na].b; idd[1]=CP[na].c; break;
                            case 2: idd[0]=CP[na].c; idd[1]=CP[na].d; break;
                            case 3: idd[0]=CP[na].d; idd[1]=CP[na].a; break;
                        }

                        if(na==-1) break;
                        pid=na;
                    }
				}
			}

   //         //選択ポイントは全て外す
			Pbs->DeselectAll();

			for(Int32 na=Pcnt; na<TpcntB; na++)
			{
			   Pbs->Select(na);
			}

			BaseSelect* ppbs= Ppoly->GetPolygonS();

			for(Int32 na=PolyCnt; na<Ppoly->GetPolygonCount(); na++)
			{
			   ppbs->Select(na);
			}

			BaseContainer bc4;
			ModelingCommandData cd4;
			bc4.SetBool( MDATA_OPTIMIZE_POINTS, true);
			bc4.SetBool( MDATA_OPTIMIZE_POLYGONS, true);
			bc4.SetBool( MDATA_OPTIMIZE_UNUSEDPOINTS, true);
			cd4.bc=&bc4;
			cd4.op = mobj[i].op;
			cd4.mode=MODELINGCOMMANDMODE_ALL;

			 if(!SendModelingCommand(MCOMMAND_OPTIMIZE, cd4))
             {
                 DeleteMem(map);
                 map=NULL;
                 return false;
             }

            ppbs->DeselectAll();

		  	for(Int32 na=0; na<mobj[i].Fcnt; na++)
			{
			   ppbs->Select(mobj[i].Snum[na]);
			}
            op->Message(MSG_UPDATE);
            
            DeleteMem(map);
			map=NULL;
			
		}
		
	}
	
	 return true;
}

//**************************************************
//
//**************************************************
Bool SnapRot::SetDrawExtubeAngle(BaseDocument* doc, BaseContainer& data,  BaseDraw* bd, BaseDrawHelp* bh, Int32 i, Matrix mr, Float rad )
{
	if(!doc) return false;
	
	BaseObject* op=mobj[i].op;
	if(!op) return false;

	PointObject *point=nullptr;
	if(op->GetType() != Opolygon )
	{
		point=(PointObject*)op->GetCache();
		if(op->GetType() == Ospline)
		{
			point=ToSpline(op);
		}
		if(!point)
		{
			point=ToPoint(op);
		}
	}
	else
	{
		point=ToPoint(op);
	}

	if(!point) return false;

	DTOOL dp;
	Int32 Scnt=mobj[i].scnt;


	bd->SetMatrix_Matrix(nullptr, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ, DRAW_Z_LOWER);
	bd->SetTransparency(0);
	
	if(Scnt)
	{
		if(op->GetType() == Opolygon)
		{
            PolygonObject* Ppoly=ToPoly(op);
            Vector* vv=point->GetPointW();

            BaseSelect* Polybs=Ppoly->GetPolygonS();
            
			for(Int32 j=0; j<Scnt; j++)
			{
				Vector v=mobj[i].vv[j]; //ポイント座標取得
				vv[mobj[i].pnum[j]]=v;
			}
			op->Message(MSG_UPDATE);
            
            Matrix Omg=op->GetMg();
            
            if(doc->GetMode() ==Mpolygons)
            {
                
                const CPolygon* CP=Ppoly->GetPolygonR();
                for(Int32 id=0; id<Ppoly->GetPolygonCount(); id++ )
                {
    				if (Polybs->IsSelected(id))
                    {
                        Int32 n=3;
                        Vector v[4];
                        v[0]=vv[CP[id].a];
                        v[1]=vv[CP[id].b];
                        v[2]=vv[CP[id].c];
                        if(CP[id].c != CP[id].d)
                        {
                            v[3]=vv[CP[id].d];
                            n=4;
                        }
                        
                        for(Int32 ni=0; ni<n; ni++)
                        {
                           // dp.printcheck(String::IntToString(ni)+"=",v[ni]);
                            v[ni]= Rad2Vector( op, v[ni],G_P1, _RADROT, EndMg );
                            v[ni]=Omg*v[ni];
                           //  dp.printcheck("     "+String::IntToString(ni)+"=",v[ni]);
                        }
                        Vector n1,n2;
                        if(n==4)
                        {
                            n1= v[2]-v[0];
                            n2= v[3]-v[1];
                        }else if(n == 3)
                        {
                            n1= v[2]-v[0];
                            n2= v[1]-v[0];
                        }
                        if(n==4)
                        {
                            Vector nv=Cross(n2, n1);
                            Vector nm[4]={ nv, nv, nv, nv};
                            
                            Vector cm=GetViewColor( VIEWCOLOR_SELECTION_PREVIEW);//ポリゴンカラー   
                            Vector fm[4] = { cm, cm, cm ,cm};
                            
                            bd->SetMatrix_Matrix(NULL, bh->GetMg());
                            bd->DrawPoly(v,fm,nm,4, NOCLIP_Z );
                        }
                        else
                        {
                            
                            Vector nv=Cross(n2, n1);
                            Vector nm[3]={ nv, nv, nv};
                            
                            Vector cm=GetViewColor( VIEWCOLOR_SELECTION_PREVIEW);//ポリゴンカラー
                            Vector fm[3] = { cm, cm, cm};
                            
                            bd->SetMatrix_Matrix(NULL, bh->GetMg());
                            bd->DrawPoly(v,fm,nm,3, NOCLIP_Z );
                            
                        }
                    }
                }
            }
            
            //回転の分割数
            Int32 subdev =data.GetInt32( MDATA_EXTUBESUBDEV);
            Float r = _RADROT / (Float)( subdev+1);
            //選択エッジの取得
            
            Modeling* mkernel=Modeling::Alloc();
            if(!mkernel) return false;
            if (!mkernel->InitObject(op)) return false;
            
            for(Int32 id=0; id<mobj[i].Ecnt; id++)
            {
                Int32 e=mobj[i].Enum[id];
                Int32 a, b;
                if(mkernel->GetOriginalEdgePoints(op,e,a,b))
                {

                    Vector va[2];
                    va[0]=vv[a];
                    va[1]=vv[b];
                    Vector vb[2];
                    vb[0]=Omg*vv[a];
                    vb[1]= Omg*vv[b];
                    for( Int32 k=1; k < subdev+2; k++)
                    {
                        Float cr=(Float) r*k;
                        Vector vc[2];
                        for(Int32 h=0; h<2; h++)
                        {
                            vc[h]= Rad2Vector( op, va[h],G_P1, cr, EndMg );
                            vc[h]=Omg*vc[h];
                        }

                        Vector n1= vb[1]-vc[0];
                        Vector n2= vb[0]-vc[1];
                        
                        Vector nv=Cross(n2, n1);
                        Vector nm[4]={ nv, nv, nv, nv};
                        
                        Vector cm=GetViewColor( VIEWCOLOR_SELECTION_PREVIEW);//ポリゴンカラー
                        Vector fm[4] = { cm, cm, cm ,cm};
                        Vector mv[]={vc[0],vc[1], vb[1],vb[0]};
                        bd->SetMatrix_Matrix(NULL, bh->GetMg());
                        bd->DrawPoly(mv,fm,nm,4, NOCLIP_Z );
                        
                        for(Int32 h=0; h<2; h++)
                        {
                            vb[h]= vc[h];
                        }
                    }
                }
            }
		}
	}
	return true;
}



  Vector  SnapRot::Rad2Vector( BaseObject* op, Vector vp,Vector Center,  Float cr, Matrix cmg )
  {

		Matrix cmr; //回転値のマトリックス
		cmr=MatrixRotY(cr);
                                
		Matrix mmg1=op->GetMg(); //グローバルマトリックス取得
		vp=mmg1*vp;				//ポイント座標をグローバル座標に変換
		vp=~cmg*vp;         //ターゲット位置を任意マトリックスで逆変換
		Vector p2=Center;   //回転中心のワールド位置逆変換
		p2=~cmg*p2;         //回転中心位置を任意マトリックスで変換
		Vector p3=vp-p2;	//グローバルポイント座標から基点座標を引いてマトリックスoffに入れる
		p3=cmr*p3;          //回転マトリックスで回転後の位置を求める
		p3+=p2;             // 
		p3=cmg*p3;
        p3=~mmg1*p3;
		return p3;
  }


//エッジの交差点を回転中心に設定
Bool  SnapRot::Edge2CenterPos(BaseDocument* doc, BaseContainer& data)
{
    if(!doc) return false;
    BaseObject* op =doc->GetActiveObject();
    if(!op || op->GetType()!= Opolygon)return false;
    DTOOL dp;
    
    PolygonObject* poly=ToPoly(op);
    
    BaseSelect* bs=poly->GetEdgeS();
    if(bs->GetCount() == 0) return false;
    
    Int32 ecnt=bs->GetCount();
    PointObject* point=ToPoint(op);
    const Vector* vv=point->GetPointR();


    Modeling* mkernel=Modeling::Alloc();
    if(!mkernel) return false;
    if (!mkernel->InitObject(op)) return false;
    
    Int32* Esels=NewMemClear(Int32,ecnt);
    if(!Esels) return false;
    
    Int32 n=0;
    for(Int32 i=0; i< poly->GetPolygonCount()*4; i++)
    {
        if (bs->IsSelected(i))
        {
            Esels[n]=i;
            n++;
        }
    }
    
    UChar* map=NewMemClear(UChar,ecnt);
    if(!map)
    {
        DeleteMem(Esels);
        Esels=NULL;
        return false;
    }
  
     Int32 ect=0;
    for(Int32 i=0; i< ecnt-1; i++)
    {
        if(!map[i])
        {
            Int32 ea= Esels[i];
            Int32 a, b;
            if(mkernel->GetOriginalEdgePoints(op,ea,a,b))
            {
                ect++;
                for(Int32 j=i+1; j<ecnt; j++)
                {
                    if(!map[j])
                    {
                        Int32 eb= Esels[j];
                        Int32 ba, bb;
                        if(mkernel->GetOriginalEdgePoints(op,eb,ba,bb))
                        {
                            if((a == ba && b==bb) || (a == bb && b ==ba))
                            {
                                map[j]=true;
                            }
                        }
                    }
                }
            }
        }
    }
    
    if(ect != 2)
    {
        DeleteMem(Esels);
        Esels=NULL;
        DeleteMem(map);
        map=NULL;
        return false;
    }
    
    Vector Lv[4];
    Int32 nc=0;
    for(Int32 i=0; i< ecnt; i++)
    {
        if(!map[i])
        {
            Int32 ea= Esels[i];
            Int32 a, b;
            if(mkernel->GetOriginalEdgePoints(op,ea,a,b))
            {
                Lv[nc]=vv[a];
                Lv[nc+1]=vv[b];
                nc+=2;
            }
        }
    }
    
    Vector crossA, crossB;
    if(dp.CrossSectionB(Lv[0], Lv[1], Lv[2], Lv[3], &crossA, &crossB, data, EndMg))
    {
        Vector cp=(crossA+crossB)/2;
        G_P1=cp;
        G_STEP = 1;
    }
    
    DeleteMem(Esels);
    Esels=NULL;
    DeleteMem(map);
    map=NULL;
    return true;
}


///////////////////////////////////////////////////////////
//平面リセット
////////////////////////////////////////////////////////
//グリッドリセット
Bool SnapRot::ResetPlaneAxis(BaseDocument* doc, BaseContainer &data)
{

	BaseObject* plane = GetWorkplaneObject(doc);
	Matrix mg;
	plane->SetMg(mg);
	data.SetInt32(IDE_Snap_AXIS, IDE_XZ);
	
	PlV[0] = PlV[1] = PlV[2] = Vector();
	PlaneSetCnt = 0;
	ResetTool();
	PlaneSet_Flag = false;
	PlaneSet_BTNFlag = false;

	EventAdd();
	
	return true;
}
/////////////////////////////////////////////
//プラグインメニュー登録
Bool RegisterSRotate()
{
	String name=GeLoadString(IDS_SnapRotate);
	return RegisterToolPlugin(ID_SRotate, name, ( PLUGINFLAG_TOOL_TWEAK_NO_HIGHLIGHT | PLUGINFLAG_TOOL_EDITSTATES), AutoBitmap("SnapRotate.tif"),GeLoadString( IDS_SnapSR_TOOL_HLP), NewObjClear(SnapRot));
}




					
					/*
					//法線ベクトルを取得
					Neighbor nbr ;
					PointObject* Pop=ToPoint(op);
					PolygonObject* Ppoly= ToPoly(op);
					if(!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;
					Vector nv= dp.GetVertexNormal(mobj[i].pnum[j],Ppoly, &nbr);

   					nv=mmg1*nv;				//ポイント座標をグローバル座標に変換
					Vector np3=cmr*nv;


				   */