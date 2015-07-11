// SScale.cpp
//////////////////////////////////////////////////////////////////////

//#include "c4d.h"

//#include "dPobjects.h" //resフォルダのヘッダーファイルをインクルード
//#include "operatingsystem.h"
#include "Dtool.h"
#include <cmath>
#include "c4d_quaternion.h"
#include "ge_sort.h"
#include "lib_modeling.h"


#define ID_SScale 1024884


class SnapSCLDataDialog: public GeModalDialog
{
	Int32 SetFlag;
	public:
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);

		virtual Bool InitDialog(void);
		virtual Bool Command(Int32 id,const BaseContainer &msg);
		Bool SetAdd( Int32 num);
};


class SnapToolDialogB : public GeDialog
{
private:
	Bool* checkFlag;

public:
	virtual Bool CreateLayout(void);
	virtual Bool InitValues(void);

	virtual Bool InitDialog(void);
	virtual Bool Command(Int32 id, const BaseContainer &msg);
	virtual Bool AskClose();
	virtual void DestroyWindow();
	void SetFlag(Bool* flag);
	void EnableGadgets(void);
};

///////////////////////////////////////////////////////////////////////////
//*****************************************************
//スナップダイアログ
//*****************************************************
Bool SnapToolDialogB::CreateLayout(void)
{
	SetTitle("Snap Setting");

	GroupBegin(0, BFH_LEFT, 1, 0, "", 0);
	GroupBorderSpace(5, 5, 5, 5);
	//GroupBegin(0,BFH_LEFT,2,0,"",0);
	//	GroupSpace(4,1);
	AddCheckbox(SNAP_ENABLEG, BFH_SCALEFIT, 0, 10, "Enable Snapping");
	AddSeparatorH(10, BFH_FIT);
	AddComboBox(SNAP_METHODG, BFH_SCALEFIT, 0, 0);
	AddChild(SNAP_METHODG, SNAP_AUTO, "Snap Auto");
	AddChild(SNAP_METHODG, SNAP_3DG, "Snap 3D");
	AddChild(SNAP_METHODG, SNAP_2DG, "Snap 2D");

	GroupBegin(0, BFH_SCALEFIT, 2, 0, "", 0);
	AddStaticText(0, BFH_LEFT, 60, 10, "Radius:", BORDER_NONE);
	AddEditNumberArrows(SNAP_RADIUSG, BFH_SCALEFIT, 30, 0);
	AddCheckbox( MDATA_VISIBLE, BFH_SCALEFIT,0,10,"Visible Only");
	GroupEnd();
	AddCheckbox(SNAP_PRE, BFH_SCALEFIT, 0, 10, "Preview");
	AddSeparatorH(10, BFH_FIT);
	AddCheckbox(SNAP_QUANTIZUNG, BFH_SCALEFIT, 0, 10, "Enable Quantizing");
	AddSeparatorH(10, BFH_FIT);

	AddCheckbox(SNAP_POINTG, BFH_SCALEFIT, 0, 10, "Vertex");
	AddCheckbox(SNAP_EDGEG, BFH_SCALEFIT, 0, 10, "Edge");
	AddCheckbox(SNAP_POLYGONG, BFH_SCALEFIT, 0, 10, "Polygon");
	AddCheckbox(SNAP_SPLINEG, BFH_SCALEFIT, 0, 10, "Spline");
	AddCheckbox(SNAP_AXISG, BFH_SCALEFIT, 0, 10, "Axis");
	AddCheckbox(SNAP_INTERSECTIONG, BFH_SCALEFIT, 0, 10, "Intersection");
	AddCheckbox(SNAP_MIDPOINTG, BFH_SCALEFIT, 0, 10, "Midpoint");
	GroupBegin(0, BFH_SCALEFIT, 2, 0, "", 0);
	AddStaticText(SNAP_SUVTEXT, BFH_LEFT, 60, 10, "Subdiv:", BORDER_NONE);
	AddEditNumberArrows(SNAP_SECTION, BFH_SCALEFIT, 30, 0);
	GroupEnd();

	AddSeparatorH(10, BFH_FIT);
	AddCheckbox(SNAP_WORKPLANEG, BFH_SCALEFIT, 0, 10, "Workplane");
	AddCheckbox(SNAP_GRIDG, BFH_SCALEFIT, 0, 10, "Grid Point");
	AddCheckbox(SNAP_GRIDLINEG, BFH_SCALEFIT, 0, 10, "Grid Line");
	AddSeparatorH(10, BFH_FIT);

	AddCheckbox(SNAP_GUIDE_SNAPG, BFH_SCALEFIT, 0, 10, "Guide Snap");
	AddCheckbox(SNAP_DYNAMICGUIDE_SNAPG, BFH_SCALEFIT, 0, 10, "Dynamic Guide");
	AddCheckbox(SNAP_PERPENDICULARG, BFH_SCALEFIT, 0, 10, "Prependicular Snap");

	GroupEnd();
	return true;
}


Bool SnapToolDialogB::InitValues(void)
{
	BaseContainer *bc = GetToolData(GetActiveDocument(), ID_SScale);
	if (!bc) return false;
	BaseDocument* doc = GetActiveDocument();
	BaseContainer bcc = SnapSettings(doc);
	//Float x=bc->GetFloat(MDATA_SNAP_VALUEX);
	SetBool(SNAP_ENABLEG, bcc.GetBool(SNAP_SETTINGS_ENABLED));


	if (bcc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_AUTO)
	{
		//	data.SetInt32(SNAP_METHODG, SNAP_AUTO);
		SetInt32(SNAP_METHODG, SNAP_AUTO);
	}
	else if (bcc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_2D)
	{
		SetInt32(SNAP_METHODG, SNAP_3DG);
	}
	else
	{
		SetInt32(SNAP_METHODG, SNAP_2DG);
	}

	SetInt32(SNAP_RADIUSG, bcc.GetInt32(SNAP_SETTINGS_RADIUS));

	SetBool(SNAP_POINTG, IsSnapEnabled(doc, SNAPMODE_POINT));
	SetBool(SNAP_EDGEG, IsSnapEnabled(doc, SNAPMODE_EDGE));
	SetBool(SNAP_POLYGONG, IsSnapEnabled(doc, SNAPMODE_POLYGON));
	SetBool(SNAP_MIDPOINTG, IsSnapEnabled(doc, SNAPMODE_MIDPOINT));
	SetBool(SNAP_GRIDG, IsSnapEnabled(doc, SNAPMODE_GRIDPOINT));
	SetBool(SNAP_GRIDLINEG, IsSnapEnabled(doc, SNAPMODE_GRIDLINE));
	SetBool(SNAP_SPLINEG, IsSnapEnabled(doc, SNAPMODE_SPLINE));
	SetBool(SNAP_AXISG, IsSnapEnabled(doc, SNAPMODE_AXIS));
	SetBool(SNAP_PERPENDICULARG, IsSnapEnabled(doc, SNAPMODE_ORTHO));

	SetBool(SNAP_QUANTIZUNG, IsSnapEnabled(doc, QUANTIZE_ENABLED));
	SetBool(SNAP_INTERSECTIONG, IsSnapEnabled(doc, SNAPMODE_INTERSECTION));
	SetBool(SNAP_WORKPLANEG, IsSnapEnabled(doc, SNAPMODE_WORKPLANE));
	SetBool(SNAP_GUIDE_SNAPG, IsSnapEnabled(doc, SNAPMODE_GUIDE));
	//SetBool( SNAP_DYNAMICGUIDE_SNAPG , bc->GetBool( SNAP_DYNAMICGUIDE_SNAPG ));
	SetBool(SNAP_DYNAMICGUIDE_SNAPG, IsSnapEnabled(doc, SNAPMODE_DYNAMICGUIDE));

	SetInt32(SNAP_SECTION, bc->GetInt32(SNAP_SECTION));
	SetBool(SNAP_PRE, bc->GetBool(SNAP_PRE));
	SetBool( MDATA_VISIBLE , bc->GetBool(MDATA_VISIBLE));
	EnableGadgets();
	//openFlag=true;
	return InitDialog();
}

Bool SnapToolDialogB::InitDialog(void)
{
	BaseContainer *bc = GetToolData(GetActiveDocument(), ID_SScale);
	if (!bc) return false;

	return true;
}

Bool SnapToolDialogB::Command(Int32 id, const BaseContainer &msg)
{
	BaseDocument* doc = GetActiveDocument();
	BaseContainer bb = SnapSettings(doc);
	BaseContainer *bc = GetToolData(GetActiveDocument(), ID_SScale);
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
		bb.SetBool(SNAP_SETTINGS_ENABLED, flag);
	}
		break;
	case SNAP_QUANTIZUNG:
	{

		Bool flag;
		GetBool(SNAP_QUANTIZUNG, flag);
		bc->SetBool(SNAP_QUANTIZUNG, flag);
		bb.SetBool(QUANTIZE_ENABLED, flag);
	}
		break;
	case SNAP_METHODG:
	{
		Int32 type;
		GetInt32(SNAP_METHODG, type);
		bc->SetInt32(SNAP_METHODG, type);
		if (type == SNAP_2DG) bb.SetInt32(SNAP_SETTINGS_MODE, SNAP_SETTINGS_MODE_2D);
		else if (type == SNAP_3DG) bb.SetInt32(SNAP_SETTINGS_MODE, SNAP_SETTINGS_MODE_3D);
		else bb.SetInt32(SNAP_SETTINGS_MODE, SNAP_SETTINGS_MODE_AUTO);
	}
		break;
	case SNAP_RADIUSG:
	{
		Int32 Num;
		GetInt32(SNAP_RADIUSG, Num);
		if (Num<1) Num = 1;
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
		EnableSnap(flag, doc, SNAPMODE_POINT);
	}
		break;
	case SNAP_EDGEG:
	{

		Bool flag;
		GetBool(SNAP_EDGEG, flag);
		bc->SetBool(SNAP_EDGEG, flag);
		EnableSnap(flag, doc, SNAPMODE_EDGE);
	}
		break;
	case SNAP_POLYGONG:
	{

		Bool flag;
		GetBool(SNAP_POLYGONG, flag);
		bc->SetBool(SNAP_POLYGONG, flag);
		EnableSnap(flag, doc, SNAPMODE_POLYGON);
	}
		break;
	case SNAP_MIDPOINTG:
	{

		Bool flag;
		GetBool(SNAP_MIDPOINTG, flag);
		bc->SetBool(SNAP_MIDPOINTG, flag);
		EnableSnap(flag, doc, SNAPMODE_MIDPOINT);
	}
		break;
	case SNAP_GRIDG:
	{

		Bool flag;
		GetBool(SNAP_GRIDG, flag);
		bc->SetBool(SNAP_GRIDG, flag);
		EnableSnap(flag, doc, SNAPMODE_GRIDPOINT);
	}
		break;
	case SNAP_GRIDLINEG:
	{

		Bool flag;
		GetBool(SNAP_GRIDLINEG, flag);
		bc->SetBool(SNAP_GRIDLINEG, flag);
		EnableSnap(flag, doc, SNAPMODE_GRIDLINE);
	}
		break;
	case SNAP_SPLINEG:
	{

		Bool flag;
		GetBool(SNAP_SPLINEG, flag);
		bc->SetBool(SNAP_SPLINEG, flag);
		EnableSnap(flag, doc, SNAPMODE_SPLINE);
	}
		break;
	case SNAP_AXISG:
	{

		Bool flag;
		GetBool(SNAP_AXISG, flag);
		bc->SetBool(SNAP_AXISG, flag);
		EnableSnap(flag, doc, SNAPMODE_AXIS);
	}
		break;
	case SNAP_PERPENDICULARG:
	{

		Bool flag;
		GetBool(SNAP_PERPENDICULARG, flag);
		bc->SetBool(SNAP_PERPENDICULARG, flag);
		EnableSnap(flag, doc, SNAPMODE_ORTHO);
	}
		break;
	case SNAP_INTERSECTIONG:
	{

		Bool flag;
		GetBool(SNAP_INTERSECTIONG, flag);
		bc->SetBool(SNAP_INTERSECTIONG, flag);
		EnableSnap(flag, doc, SNAPMODE_INTERSECTION);
	}
		break;
	case SNAP_WORKPLANEG:
	{

		Bool flag;
		GetBool(SNAP_WORKPLANEG, flag);
		bc->SetBool(SNAP_WORKPLANEG, flag);
		EnableSnap(flag, doc, SNAPMODE_WORKPLANE);
	}
		break;

	case SNAP_GUIDE_SNAPG:
	{

		Bool flag;
		GetBool(SNAP_GUIDE_SNAPG, flag);
		bc->SetBool(SNAP_GUIDE_SNAPG, flag);
		EnableSnap(flag, doc, SNAPMODE_GUIDE);
	}
		break;
	case SNAP_DYNAMICGUIDE_SNAPG:
	{

		Bool flag;
		GetBool(SNAP_DYNAMICGUIDE_SNAPG, flag);
		bc->SetBool(SNAP_DYNAMICGUIDE_SNAPG, flag);
		EnableSnap(flag, doc, SNAPMODE_DYNAMICGUIDE);
	}
		break;
	case SNAP_SECTION:
	{
		Int32 Num;
		GetInt32(SNAP_SECTION, Num);
		if (Num<2) Num = 2;
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
	EventAdd();
	*checkFlag = true;
	EnableGadgets();
	return true;
}

void SnapToolDialogB::EnableGadgets()
{

	Bool flag;
	GetBool(SNAP_ENABLEG, flag);

	Enable(SNAP_POINTG, flag);
	Enable(SNAP_EDGEG, flag);
	Enable(SNAP_POLYGONG, flag);
	Enable(SNAP_SPLINEG, flag);
	Enable(SNAP_AXISG, flag);
	Enable(SNAP_INTERSECTIONG, flag);



	Enable(SNAP_WORKPLANEG, flag);
	Bool flagB;
	GetBool(SNAP_WORKPLANEG, flagB);
	if (flag && flagB)
	{
		Enable(SNAP_GRIDG, true);
		Enable(SNAP_GRIDLINEG, true);
	}
	else
	{
		Enable(SNAP_GRIDG, false);
		Enable(SNAP_GRIDLINEG, false);
	}
	Enable(SNAP_GUIDE_SNAPG, flag);
	GetBool(SNAP_GUIDE_SNAPG, flagB);
	if (flag && flagB)
	{
		Enable(SNAP_DYNAMICGUIDE_SNAPG, true);
		Enable(SNAP_PERPENDICULARG, true);
	}
	else
	{
		Enable(SNAP_DYNAMICGUIDE_SNAPG, false);
		Enable(SNAP_PERPENDICULARG, false);
	}


	Bool flagC, flagD;
	GetBool(SNAP_EDGEG, flagB);
	GetBool(SNAP_POLYGONG, flagC);
	GetBool(SNAP_SPLINEG, flagD);
	if (flag && (flagB || flagC || flagD))Enable(SNAP_MIDPOINTG, true);
	else Enable(SNAP_MIDPOINTG, false);

	Enable(SNAP_SECTION, flag);
	Enable(SNAP_SUVTEXT, flag);
}

void SnapToolDialogB::DestroyWindow()
{
	//pQuickTab=NULL;
}

Bool SnapToolDialogB::AskClose()
{

	//openFlag=false;
	return false;
}

void SnapToolDialogB::SetFlag(Bool* flag)
{
	checkFlag = flag;
}

//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////
//クラス定義
class SnapScl : public DescriptionToolData
{
private:
	Vector G_markpoint; //ポインター座標
	Vector G_P1;		//１点目のポイント
	Vector G_P2;		//2点目のポイント

	Int32 G_STEP;		//進行ステップ
	Int32 G_SnapFlag;  //スナップ判定

	Vector G_nvec;    //法線ベクトル
	Mobj *mobj;
	Bool AxisFlag;

	Float rn;
	Float rp;

	Int32 ObjCnt;
	Bool getFlag;
	Bool AltKey;


	Bool PlaneSet_BTNFlag;
	Int32 PlaneSetCnt;
	Bool PlaneSet_Flag;
	Vector PlV[3];

	Bool CtrlOn;
	//PolygonObject* o;
	Vector lineV[3];
	Int32 EdgeID;
	Vector MMP;
	Vector shiftPos;
	Int32 Odlg_flag;

	SnapSCLDataDialog dlg;

	Int32 Guide;
	Int32 PlaneNum;

	BaseObject* _Starget;
	Int32 SnapID;

	Bool CtrlKey;
	Bool ShiftKey;

	Float HUDOffset;
    Vector $VectorXYZ[3];

	Bool SnpDialogFlag;
	Vector Aline[3];

	Int32 MenuDrag;
	Vector MenuV1;
	Int32 MenuOpen1;
	SnapToolDialogB Sndlg;

	

	Bool checkFlag;
	Vector MemoPos;	 //Ctrlキー押した時のマウスカーソル座標用変数
	Float _MX; //マウスカーソル座標(２D)
	Float _MY; //マウスカーソル座標(２D)
    
	Bool InitFlag;


 	Matrix EndMg;		//最終マトリックス
	Matrix FaceMg;		//ポリゴン法線マトリックス

	Bool RectPointVector( BaseObject *op, Int32 i );
	Bool SetPointScale( BaseObject *op, Int32 i, Vector Scl,Matrix A_mg );
	Bool RectPolygonVector(BaseObject *op, Int32 i );
	Bool RectEdgeVector(BaseObject *op, Int32 i );
	Bool AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE type);
	Bool SetPlaneAxis(BaseDocument* doc);
	Bool ResetPlaneAxis(BaseDocument* doc, BaseContainer &data);
	Bool GetSELPoly(BaseObject *op,  Int32 num, Int32 id);
	Bool GetSELEdge(BaseObject *op,  Int32 num, Int32 id);
	Bool InteractiveValue(BaseDocument *doc, BaseContainer &data);
	void ResetTool();
	Bool Set3pointPlane(BaseDocument* doc, BaseContainer &data );

	Bool DispAXIS(BaseDocument *doc, BaseDraw *bd, BaseDrawHelp *bh, BaseContainer &data);
	void Priinit(BaseContainer& data, Bool flag );
	void SaveData(HyperFile* hf, BaseContainer& data);
	void DefoultSet( BaseContainer& data);
	void ReadData(BaseContainer bc, BaseContainer& data);
	
public:
	SnapScl(); //コンストラクタ
	virtual ~SnapScl(); //デストラクタ
	virtual Int32 GetToolPluginId() { return ID_SScale; }
	virtual const String GetResourceSymbol() { return String("SnapScale"); } //resファイルで記述されている名前を入れる
	virtual Bool InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt);
	virtual void FreeTool( BaseDocument* doc, BaseContainer& data) ;
	virtual Int32 GetState(BaseDocument *doc);
	virtual Bool MouseInput( BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg) ;
	virtual TOOLDRAW Draw(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, BaseThread* bt, TOOLDRAWFLAGS flags);
	virtual Bool GetCursorInfo(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, Float x, Float y, BaseContainer &bc) ;
	virtual void InitDefaultSettings(BaseDocument *doc, BaseContainer &data);
	virtual Bool DoCommand( ModelingCommandData &mdat);
	virtual Bool GetDEnabling(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_ENABLE flags, const BaseContainer* itemdesc);
	virtual Bool Message(BaseDocument* doc, BaseContainer& data, Int32 type, void* t_data);
	virtual Bool KeyboardInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg);
	virtual Bool SetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags);
	virtual Bool GetDDescription(BaseDocument* doc, BaseContainer& data, Description* description, DESCFLAGS_DESC& flags);
	virtual Bool GetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags);

	private:
		SnapCore* _snap;			// the snap object for this tool
};
////////////////////////////////////////////////////////////////////////







SnapScl::SnapScl()
{
	G_markpoint=Vector();
	EndMg=Matrix();
	G_P1=G_P2 =  Vector();

	G_STEP=NULL;
	G_SnapFlag=NULL;
	G_nvec=Vector();
//	CtrlKey=false;
	mobj=nullptr;
	AxisFlag=false;
	//FaceMg=Matrix();
	ObjCnt=0;
	rn=0.0;
	rp=0.0;

	getFlag=false;
	MenuDrag = -1;
	MenuV1 = Vector();

	PlaneSet_BTNFlag=false;
	PlaneSetCnt=0;
	PlaneSet_Flag=false;
	PlV[0]=PlV[1]=PlV[2]=Vector();
	Odlg_flag=-1;
	PlaneNum=IDE_XZ;
	HUDOffset=40;
	InitFlag= false;
	_snap = nullptr;
}
//デストラクタ
SnapScl::~SnapScl()
{
	if (_snap)
		SnapCore::Free(_snap);
	_snap = nullptr;
}
//--------------------------------------------------------------------------------------------------------------------------
//tool終了時実行
void SnapScl::FreeTool( BaseDocument* doc, BaseContainer& data)
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
void SnapScl::InitDefaultSettings( BaseDocument *doc, BaseContainer &data)
{
	
	rn=rp=0.0;
	AxisFlag=false;

	//GePrint("InitDefaultSettings");
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
Bool SnapScl::GetDEnabling(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_ENABLE flags, const BaseContainer* itemdesc)
{
	if (!doc)     return false;

	switch (id[0].id)
	{


		case MDATA_SETAXIS:
			{
				if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc) || PlaneSet_Flag || G_STEP) return false;
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
		//case IDB_BTN_SET:
		//	{
		//		if(G_STEP==2) return true;
		//		else return false;
		//	}
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
	case IDB_BTN_SETSCL:
		{
			if(G_STEP==2) return true;
			else return false;
		}
	case IDSCL_X:
		{
			if(data.GetBool(IDB_X)) return true;
			else return false;
		}
	case IDSCL_Y:
		{
			if(data.GetBool(IDB_Y)) return true;
			else return false;
		}
	case IDSCL_Z:
		{
			if(data.GetBool(IDB_Z)) return true;
			else return false;
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
			if(data.GetBool(SNAP_GUIDE_SNAPG)&& data.GetBool(SNAP_ENABLEG)&& data.GetBool(SNAP_DYNAMICGUIDE_SNAPG))
			{
				return (SNAP_PERPENDICULARG >= 0L);
			}
			else
			{
				return ( SNAP_PERPENDICULARG <= 0L);
			}
		}
	default:
		break;
     }

	return SUPER::GetDEnabling(doc, data, id, t_data, flags, itemdesc);
}
//--------------------------------------------------------------------------------------------------------------------------
//現状取得
Int32 SnapScl::GetState(BaseDocument *doc)
{

	return CMD_ENABLED;
}
//--------------------------------------------------------------------------------------------------------------------------
//初期化
Bool SnapScl::InitTool(BaseDocument* doc, BaseContainer& data, BaseThread* bt)
{
	 DTOOL DP;
    SYSTEMINFO    stype =        GeGetSystemInfo();
	if ( stype & (SYSTEMINFO_SAVABLEDEMO|SYSTEMINFO_SAVABLEDEMO_ACTIVE) || stype & SYSTEMINFO_COMMANDLINE  )
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

void SnapScl::ResetTool()
{

	G_STEP = 0;
	G_P1=G_P2=Vector();

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
	DeleteMem(mobj);
	mobj=nullptr;
	ObjCnt=0;
	
	rn=rp=0.0;
}
//--------------------------------------------------------------------------------------------------------------------------
//モデリングコマンド
Bool SnapScl::DoCommand( ModelingCommandData &mdat)
{
	//return ModelingAct_for$MCommand( &mdat) ;
		return true;
}

//--------------------------------------------------------------------------------------------------------------------------
//キーインプット
Bool SnapScl::KeyboardInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, EditorWindow *win, const BaseContainer &msg)
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
		ResetTool();
	}
	EventAdd();
	return false;
}


//--------------------------------------------------------------------------------------------------------------------------
//マウスインプット
Bool SnapScl::MouseInput(BaseDocument *doc, BaseContainer &data, BaseDraw *bd,EditorWindow *win, const BaseContainer &msg)
{

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

	doc->StartUndo();
	if(CtrlOn)
	{


		if(EdgeID== 1 || EdgeID== 4)
		{
			if(data.GetBool(IDB_X))
			{
				data.SetBool(IDB_X, false);
			}
			else
			{
				data.SetBool(IDB_X, true);
			}
		}
		else if(EdgeID== 3 || EdgeID== 8)
		{
			if(data.GetBool(IDB_Y))
			{
				data.SetBool(IDB_Y, false);
			}
			else
			{
				data.SetBool(IDB_Y, true);
			}
		}
		else if(EdgeID== 7 || EdgeID== 9)
		{
			if(data.GetBool(IDB_Z))
			{
				data.SetBool(IDB_Z, false);
			}
			else
			{
				data.SetBool(IDB_Z, true);
			}
		}

		else if(EdgeID==100)
		{
			if(Odlg_flag == 2 || Odlg_flag == 3)
			{
				dlg.Close(false);
			}
			dlg.SetAdd(1);

			//Odlg.Open(DLG_TYPE_ASYNC_POPUPEDIT,ID_MODELING_SnapALIGN,mx,my+100,0,0,0);
			dlg.Open(-1,-1,0,0);
			Odlg_flag=1;
		}
		else if(EdgeID==200)
		{
			if(Odlg_flag == 1 || Odlg_flag == 3)
			{
				dlg.Close(false);
			}
			dlg.SetAdd(2);
		//	Odlg.Open(DLG_TYPE_ASYNC_POPUPEDIT,ID_MODELING_SnapALIGN,mx,my,0,0,0);
			dlg.Open(-1,-1,0,0);
			Odlg_flag=2;
		}
		else if(EdgeID==300)
		{
			if(Odlg_flag == 1 || Odlg_flag == 2)
			{
				dlg.Close(false);
			}
			dlg.SetAdd(3);
			//Odlg.Open(DLG_TYPE_ASYNC_POPUPEDIT,ID_MODELING_SnapALIGN,mx,my,0,0,0);
			dlg.Open(-1,-1,0,0);
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
            data.SetBool(IDB_X, _Xflag);
            data.SetBool(IDB_Y, _Xflag);
            data.SetBool(IDB_Z, !_Xflag);
            
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
            data.SetBool(IDB_Z, _Yflag);
            data.SetBool(IDB_Y, _Yflag);
            data.SetBool(IDB_X, !_Yflag);
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
            data.SetBool(IDB_Z, _Zflag);
            data.SetBool(IDB_X, _Zflag);
            data.SetBool(IDB_Y, !_Zflag);
        }

		//DTOOL DP;
		//	MenuDrag=-1;
		Float dx, dy;

		BaseContainer bc;
		BaseContainer device;
		//Float rx=mx;
		MenuV1 = Vector(mx, my, 0);
		MenuDrag = 0;
		Vector DragPos;
		////マウスドラッグスタート
		win->MouseDragStart(button, mx, my, MOUSEDRAGFLAGS_DONTHIDEMOUSE | MOUSEDRAGFLAGS_NOMOVE);
		while (win->MouseDrag(&dx, &dy, &device) == MOUSEDRAGRESULT_CONTINUE)		//ドラッグ距離の取得
		{

			bc = BaseContainer();
			win->BfGetInputEvent(BFM_INPUT_MOUSE, &bc);
#ifdef __PC
			{
				if (dx == 0.0 && dy == 0.0) continue;
			}
#else
						{
							if (dx < 1 && dx >-1 && dy < 1 && dy >-1) continue;
						}
#endif
			mx += dx;
			my += dy;

			G_markpoint = bd->SW(Vector(mx, my, 500));

			Vector MenuV2 = Vector(mx, my, 0);

			MenuV2 = MenuV2 - MenuV1;
			Matrix m0;
			Float rad = DP.VectorToRad(MenuV2, Vector(1, 0, 0), m0);

			rad = Deg(rad);
			Float LS = Len(MenuV2);

			if (rad < 45 && rad >-45 && LS > 50)
			{
				MenuDrag = 1;
			}
			else if (rad < 135 && rad >45 && LS > 50)
			{
				MenuDrag = 2;
			}
			else if (rad > -135 && rad <-45 && LS > 50) //Move Mode
			{
				MenuDrag = 3;
			}
			else if (LS > 50)  //Offset Setting
			{
				MenuDrag = 4;
			}
			else
			{
				MenuDrag = 0;

			}
			DragPos = Vector(mx, my, 0);

			DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD | DRAWFLAGS_NO_ANIMATION);

		}
		if (win->MouseDragEnd() == MOUSEDRAGRESULT_FINISHED)
		{

			switch (MenuDrag)
			{
			case 1: //スナップ設定パネルオープン
				//if(!CheckSnapSettingP( doc,data)) return false;
				//	Sndlg.Open(DLG_TYPE_ASYNC_POPUP_RESIZEABLE,ID_MODELING_SnapALIGN,DragPos.x,DragPos.y,0,0,0);
				//if (AxisOnFlag && data.GetBool(SNAP_ENABLEG))
				//{
				//	BaseContainer bbc = SnapSettings(doc);
				//	bbc.SetBool(SNAP_SETTINGS_ENABLED, true);
				//	SnapSettings(doc, bbc);
				//}
				
				Sndlg.SetFlag(&SnpDialogFlag);
				Sndlg.Open(DLG_TYPE_ASYNC_POPUP_RESIZEABLE, ID_SScale, (Int32)mx, (Int32)my, 0, 0, 0);
				//	Sndlg.Open(-1,-1,0,0);

				MenuOpen1 = 1;
				break;

			//case 2:
			//	data.SetInt32(MDATA_SNAP_MODE, MDATA_CH_ALIGN);
			//	break;
			//case 3:
			//	data.SetInt32(MDATA_SNAP_MODE, MDATA_CH_POSFIX);
			//	break;
			case 4:

				MenuOpen1 = 4;
				break;
			}
			EventAdd();
			MenuDrag = -1;
		}
	}
	else
	{
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
					if( G_SnapFlag == 1)
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

			//**********************************************
		}
		else
		{

			if(G_STEP==0){
				//アンドォスタート
				//doc->StartUndo();
				//一点目のポイント座標を取得
				G_P1= Vector();
				G_P1=G_markpoint;
				G_STEP = 1;
				////基準平面マトリックスを取得
				//EndMg = DP.GetPlaneMatrix(doc, bd, data, PlaneSet_Flag, FaceMg);
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
					Int32 cnt=arr->GetCount();
					//GePrint("cnt-"+Int32ToString(cnt));
					if(cnt)
					{
						//ObjCnt=cnt;
						AddUndo(doc, arr, UNDOTYPE_CHANGE);
						mobj=nullptr;
							//選択ﾎﾟｲﾝﾄを格納用変数
					//	mobj=(Mobj *) GeAlloc(cnt * sizeof(Mobj));
						mobj= NewMemClear( 	Mobj, cnt);

						if(!mobj) goto Error;
								
						Int32 gh=0;
						for(Int32 i=0; i<cnt; i++)
						{
							BaseObject *op=(BaseObject *)arr->GetIndex(i);
							if(!op) goto Error;
							if(op->GetType() == Opolygon || op->GetType() == Ospline || op->GetType() == Offd )
							{
								mobj[gh].op=op;

								if(mode== Mpoints)
								{
									if(!RectPointVector( op, gh)) goto Error;
								}
								else if(mode== Medges)
								{
									if(!RectEdgeVector(op, gh )) goto Error;
								}
								else if( mode == Mpolygons )
								{
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
					ObjCnt=cnt;
					if(cnt)
					{
						AddUndo(doc, arr, UNDOTYPE_CHANGE);
						mobj=nullptr;
						//mobj=(Mobj *) GeAlloc(cnt * sizeof(Mobj));
						mobj= NewMemClear(Mobj, cnt);
						if(!mobj) goto Error;
					//	Int32 gh=0;
						for(Int32 i=0; i<cnt; i++)
						{
							BaseObject *op=(BaseObject *)arr->GetIndex(i);
							if(!op) goto Error;
							mobj[i].Mmg=op->GetMg();
							mobj[i].op=op;
							if(!RectPointVector( op, i)) goto Error;							
								
						}
					}
				}

				G_P2=Vector();
				G_P2=G_markpoint;

				G_STEP = 2;

				//ResetTool();
			}
			else if(G_STEP == 2)
			{
				//スケールリセット
				Int32 mode = doc->GetMode();
				if(!ObjCnt) return false;
				if(ObjCnt)
				{
					for(Int32 i=0; i<ObjCnt; i++)
					{
						//BaseObject *op=(BaseObject*)arr->GetIndex(i);
						BaseObject *op=mobj[i].op;
						if(!op) goto Error;

						if(op->GetType() == Opolygon || op->GetType() == Ospline)
						{
							if(mode == Mmodel)
							{
								BaseContainer bc;
								bc.SetBool(MDATA_RESETSYSTEM_COMPENSATE,true);
								bc.SetBool(MDATA_RESETSYSTEM_RECURSIVE,true);
								ModelingCommandData cd;
								cd.doc = doc;
								cd.op = op;
								cd.bc = &bc;
								cd.mode=MODELINGCOMMANDMODE_ALL;
								if (!SendModelingCommand(MCOMMAND_RESETSYSTEM, cd))
								{
									return false;
								}
							}
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
TOOLDRAW SnapScl::Draw(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh, BaseThread* bt, TOOLDRAWFLAGS flags)
{
	if (!(flags & TOOLDRAWFLAGS_HIGHLIGHT)) 
	return TOOLDRAW_0;

	DTOOL DP;
//	BaseDraw *bbd = doc->GetActiveBaseDraw();
	//Int32 view =	bbd->GetProjection();

	GeData lw = bd->GetParameterData(DRAW_PARAMETER_LINEWIDTH);
	bd->SetMatrix_Matrix(nullptr, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->SetTransparency(0);


	if(CtrlOn)
	{
		
		if (!DispAXIS(doc, bd, bh, data)) return TOOLDRAW_0;

		bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);

		bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));		
		bd->SetPen(Vector(255,225,0));
		Vector PSp=bd->WS(MMP);
		Float ra=data.GetInt32(SNAP_RADIUSG);
		 bd->SetMatrix_Matrix(nullptr, bh->GetMg());
		bd->DrawCircle2D((Int32)PSp.x, (Int32)PSp.y, ra);

		bd->SetPen(Vector(1.0, 0, 0));


		bd->DrawHandle(MMP, DRAWHANDLE_MIDDLE, 0);
		DTOOL dp;
		if(data.GetBool(IDB_X))
		{
			String sx=String::FloatToString(data.GetFloat(IDSCL_X))+"  ";
			dp.DrawTextA(sx ,lineV[0] ,HUDOffset , bd, EdgeID );
		}
		if(data.GetBool(IDB_Y))
		{
			String sy=String::FloatToString(data.GetFloat(IDSCL_Y) )+"  ";
			dp.DrawTextA(sy ,lineV[1] ,HUDOffset , bd, EdgeID );
		}
		if(data.GetBool(IDB_Z))
		{
			String sz=String::FloatToString(data.GetFloat(IDSCL_Z ))+"  ";
			dp.DrawTextA(sz ,lineV[2] ,HUDOffset, bd, EdgeID );
		}

		if (MenuDrag != -1)
		{
			//DTOOL dp;

			bd->SetMatrix_Matrix(NULL, bh->GetMg());
			bd->SetPen(Vector(1.0, 0.0, 1.0));
			bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.5));

			Vector MenuV2 = bd->WS(G_markpoint);
			//MenuV2.z=0;
			bd->DrawLine2D(MenuV1, MenuV2);


			if (MenuDrag == 1)
			{
				dp.DrawTextA("Snap Setting  ", G_markpoint, HUDOffset, bd, 1);
			}
			//if (MenuDrag == 2)
			//{
			//	dp.DrawTextA("Align Mode  ", G_Mark, HUDOffset, bd, 1);
			//}
			//if (MenuDrag == 3)
			//{
			//	dp.DrawTextA("Move Mode  ", G_Mark, HUDOffset, bd, 1);
			//}
			//if (MenuDrag == 4)
			//{
			//	dp.DrawTextA("Option Setting  ", G_markpoint, HUDOffset, bd, 1);
			//}
		}
	}
	else
	{
		DP.DispMarker(doc, bd, bh, data, EndMg, G_markpoint, PlaneSet_Flag, G_SnapFlag, PlaneSet_BTNFlag, FaceMg, _MX, _MY, 1);
		if(PlaneSet_BTNFlag)
		{
			if(PlaneSetCnt==1)
			{
				bd->SetMatrix_Matrix(nullptr, bh->GetMg());
				bd->SetPen(Vector(0.0,1.0,1.0));
				bd->DrawHandle(PlV[0],DRAWHANDLE_BIG, 0);
				bd->SetPen(Vector(0.0,0.0,1.0));
				bd->DrawLine(PlV[0],G_markpoint, 0);
			}
			else if(PlaneSetCnt==2)
			{
				bd->SetMatrix_Matrix(nullptr, bh->GetMg());
				bd->SetPen(Vector(0.0,1.0,1.0));
				bd->DrawHandle(PlV[0],DRAWHANDLE_BIG, 0);
				bd->DrawHandle(PlV[1],DRAWHANDLE_BIG, 0);
				bd->SetPen(Vector(0.0,0.0,1.0));
				bd->DrawLine(PlV[0],PlV[1], 0);
				bd->DrawLine(PlV[0],G_markpoint,0);
			}
		}
		else
		{
			if(G_STEP==1)
			{		
				bd->SetMatrix_Matrix(nullptr, bh->GetMg());
				bd->SetPen(Vector(1.0,0.0,0.5));
				bd->DrawHandle(G_P1,DRAWHANDLE_BIG,0); //１点目のポイント
				bd->SetPen(Vector(0.0,1.0,0.5));
				bd->DrawLine(G_P1,G_markpoint, 0);
			}
			else if(G_STEP==2)
			{
				//
				bd->SetMatrix_Matrix(nullptr, bh->GetMg());
				Vector spos=G_markpoint;
				bd->SetPen(Vector(0.0,0.0,1.0));
				bd->DrawHandle(G_P1,DRAWHANDLE_BIG, 0);
				bd->SetPen(Vector(0.0,1.0,0.5));
				bd->DrawLine(G_P1,spos, 0);
				bd->SetPen(Vector(1.0,0.0,1.0));
				bd->DrawHandle(spos,DRAWHANDLE_BIG,0);

				Int32 view = doc->GetActiveBaseDraw()->GetProjection();
                
				 Matrix A_mg =  EndMg;

 
				//マトリックスの位置をリセット
				A_mg.off = Vector();

				//スケール値を求める
				Vector Scl;
				Vector va=G_P1;
				Vector vb=G_P2;
				Vector vc=G_markpoint;

				//座標をA_mg基準でローカル化
				va = ~A_mg * va;
				vb = ~A_mg * vb;
				vc = ~A_mg * vc;


				Vector sA = vb-va;
				Vector sB = vc-va;
			//	GePrint("view=" + String::IntToString(view));
				if (view == 4 || view == 5) //front &back
				{
					sA.z = 1;
					sB.z = 1;
				}

				if (view == 3 || view == 2) //right&left
				{
					sA.x = 1;
					sB.x = 1;
				}
				if (view == 6 || view == 7) //top&bottom
				{
					sA.y = 1;
					sB.y = 1;
				}

				//DP.printcheck(" sA=", sA);
				//DP.printcheck(" sB=", sB);
				if(!ShiftKey)
				{
					Float sx=0;
					Float sy=0;
					Float sz=0;
					if (Abs(sA.x) > 0.0001)
					{
						sx = sB.x / sA.x;
					}
					else
					{
						sx = 1;
					}

					if  (Abs(sA.y) > 0.0001)
					{
						sy = sB.y / sA.y;
					}
					else
					{
						sy = 1;
					}
					if (Abs(sA.z) > 0.0001)
					{
					//	GePrint("gggg");
						sz = sB.z / sA.z;
					}
					else
					{
						sz = 1;
					}

					Scl =Vector( sx,sy,sz);

					if(Abs(vb.x - vc.x) < 0.01) Scl.x=1;
					if(Abs(vb.y - vc.y) < 0.01) Scl.y=1;
					if(Abs(vb.z - vc.z) < 0.01) Scl.z = 1;

				}
				else
				{
					Vector Ctv1=sA;
					Vector Ctv2=sB;

					Ctv1.y=0;
					Ctv2.y=0;
					Float rA=Len(Ctv1);
					Float rB=Len(Ctv2);
					Float Lsc=rB/rA;
					Scl =Vector( Lsc,Lsc,Lsc);
				}
				//DP.printcheck("***Scl=", Scl);
				
				//XYZ軸のスケール値を等しくする
				if(data.GetInt32(IDG_LOCK) == IDB_LOCKXYZ)
				{
					Float sc=Scl.x;
					if(sc < Scl.z) sc=Scl.z;
				//	if(sc < Scl.y) sc=Scl.y;
					Scl.x=sc;
					Scl.y=sc;
					Scl.z=sc;
				}

				
				Vector vscl=Vector();
				
                if(!data.GetBool(IDB_X)) Scl.x=1;
                if(!data.GetBool(IDB_Y)) Scl.y=1;
                if(!data.GetBool(IDB_Z)) Scl.z=1;


				//アトリビュートに表示
				if(!AltKey)
				{
					data.SetFloat(IDSCL_X, Scl.x);
					data.SetFloat(IDSCL_Y, Scl.y);
					data.SetFloat(IDSCL_Z, Scl.z);
				}
                
                vscl=Scl;
				//DP.printcheck(" *******Scl=", Scl);


				//基準平面でのスケールマトリックスを作成
				Matrix Smg=MatrixScale(Scl);

				
				//現在のモードを取得
				Int32 mode = doc->GetMode();
				if( mode== Mpoints || mode == Medges || mode == Mpolygons )
				{
					if(ObjCnt)
					{
						for(Int32 i=0; i<ObjCnt; i++)
						{
							BaseObject *op=mobj[i].op;
							if(!op) goto Error;

							if(!SetPointScale(op, i, Scl, A_mg  )) goto Error;
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

                            //オブジェクトのマトリックスを取得
							Matrix omg=mobj[i].Mmg;


							//オブジェクトのグローバル座標を取得
							Vector Gpos=omg.off;


                            //座標を記憶
							Vector p1=~A_mg*Gpos;
							//DP.printcheckR("p1 = ", p1);
                            //オブジェクト位置から１番クリック位置の差を取得
							Vector p3=p1-va;
                            
                            //差をスケール変換で距離を求める
							p3=Smg*p3;
                            //１番クリック位置をプラスしてオブジェクトの位置を求める
							p3=va+p3;
							//グローバル変換
							p3 = A_mg*p3;


							//基準平面、オブジェクトのグローバル回転を取得
							Vector PRot = MatrixToHPB(A_mg, ROTATIONORDER_DEFAULT);
							Vector ORot = MatrixToHPB(omg, ROTATIONORDER_DEFAULT);

							Int32 n=0;


							//	基準平面からオブジェクト回転の差を求める
							Vector cr = PRot - ORot;
						//	DP.printcheckR("cr=",cr);
							
							
							Float fx = ((Int32)Deg(cr.x)) % 90;
							Float fy = ((Int32)Deg(cr.y)) % 90;
							Float fz = ((Int32)Deg(cr.z)) % 90;



							if(fx == 0 ) n+=1;
							if(fy == 0 ) n+=1;
							if(fz == 0 ) n+=1;

							//GePrint(" n="+String::IntToString(n));

							 if(n > 2)
							 {
								 Matrix pmg = omg;
								 pmg.off = Vector();

                                 //オブジェクトの回転を取得し、回転用マトリックスを作成
								Vector mRot = MatrixToHPB(pmg, ROTATIONORDER_DEFAULT);
								Matrix mrg= HPBToMatrix(mRot, ROTATIONORDER_DEFAULT);

                                //基準マトリックスでスケールマトリックスをグローバル化
								Matrix ms = Smg * A_mg;
								Scl = Vector(Len(ms.v1), Len(ms.v2), Len(ms.v3));
                              //  DP.printcheck("SCL = ", Scl);
                                Smg = MatrixScale(Scl);
                                Smg= ~pmg*Smg;
								omg = mrg * Smg;
							 }
							 else
							 {

  								if( op->GetType() == Opolygon || op->GetType() == Ospline)
								{
								//	GePrint("-------------------");
									AutoAlloc<Modeling> mod;
									if (!mod || !mod->InitObject(op)) return TOOLDRAW_0;
									PointObject* pp=ToPoint(op);
									Vector* vv=mobj[i].vv;
									Int32 cnt = pp->GetPointCount();
									Matrix pmg = omg;
									pmg.off = Vector();
									for(Int32 j=0; j< cnt; j++)
									{
										Vector v1= vv[j];
										//DP.printcheck("v1=", v1);
										v1=pmg*v1;	 //グローバル変換
									//	DP.printcheck("Global v1=", v1);
										v1=~A_mg*v1; //平面マトリックスでローカル変換
									//	DP.printcheck("Local v1=", v1);
										v1=	Smg*v1;	 //スケール変換
										//DP.printcheck("scale v1=", v1);
										v1= A_mg*v1; //	平面座標をグローバル変換

									//	DP.printcheck("   Global  v1=", v1);
										v1=~pmg*v1;	 //オブジェクト内座標に変換
									//	DP.printcheck("            v1=", v1);
										if (!mod->SetPoint(op, j, v1, MODELING_SETPOINT_FLAG_EMPTY)) return TOOLDRAW_0;
									}
									if (!mod->Commit(op, MODELING_COMMIT_UPDATE)) return TOOLDRAW_0;
								}
								else
								{
									Matrix pmg = omg;
									pmg.off = Vector();

									//オブジェクトの回転を取得し、回転用マトリックスを作成
									Vector mRot = MatrixToHPB(pmg, ROTATIONORDER_DEFAULT);
									Matrix mrg= HPBToMatrix(mRot, ROTATIONORDER_DEFAULT);

									//基準マトリックスでスケールマトリックスをグローバル化
									Matrix ms = Smg * A_mg;
									Scl = Vector(Len(ms.v1), Len(ms.v2), Len(ms.v3));
									//  DP.printcheck("SCL = ", Scl);
									Smg = MatrixScale(Scl);
									Smg= ~pmg*Smg;
									omg = mrg * Smg;
                                }
							 }

  							omg.off = p3;	//位置を与える
							op->SetMg(omg);
						}
					}
					
				}
				
			}
		}

	}

	return TOOLDRAW_HANDLES|TOOLDRAW_AXIS;

Error:
	 ResetTool();
	return TOOLDRAW_0;
}
  
//--------------------------------------------------------------------------------------------------------------------------
//マウスカーソル
Bool SnapScl::GetCursorInfo(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, Float x, Float y, BaseContainer &bc)
{

	if( bc.GetId() == BFM_CURSORINFO_REMOVE) return true;
	bc.SetString( RESULT_BUBBLEHELP, GeLoadString( IDS_SnapSC_TOOL_HLP));


	DTOOL DP;
	//***********************************************
	//スナップ設定パネル表示時にスナップ設定同期
	if (SnpDialogFlag)
	{
		if (!DP.SnapSync(doc, data, PlaneSet_BTNFlag)) return false;
		EventAdd();
		SnpDialogFlag = false;
	}
	//***********************************************


	_MX=x;
	_MY=y;
	//Ctrlキー取得
	BaseContainer state;
	GetInputState(BFM_INPUT_KEYBOARD, BFM_INPUT_QUALIFIER, state);
	Int32 qual = state.GetInt32(BFM_INPUT_QUALIFIER);
	AltKey=false;
	CtrlKey=false;
	ShiftKey = false;
	
	if (G_STEP == 0)
	{
		//作業平面設定と基準マトリックスの取得
		EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg, 2);
	}

	if(qual == QALT)   //ALTキーで描画停止
	{
		AltKey=true;
		//LocV=G_Mark;
	}

	else if(qual== QCTRL) //CTRLキー：アクシズモデル表示
	{
		MemoPos = G_markpoint;
		EdgeID = -1;

		if(!CtrlOn)
		{
			shiftPos=bd->SW(Vector(x,y,500.0));
		}
		CtrlOn=true;
		AltKey= true;
		CtrlKey=true;
		
		if(DP.FuzzyPointInRange(data, bd, shiftPos,x,y))
		{
			EdgeID=30;
		}
		else
		{

			BaseDraw *bbd = doc->GetActiveBaseDraw();
			Int32 view =	bbd->GetProjection();


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

			if (EdgeID == -1)
			{
				if (!DP.PointerToAXISEdgeCheck(bd, data, x, y, Aline, shiftPos, &EdgeID))
				{
					EdgeID = -1;
				}
			}

			if(view==6 && EdgeID==0)
			{
				EdgeID=4;
			}
			if(view==2 || view==3)
			{
				data.SetBool(IDB_X ,false);
			}
			if(view==4 || view==5)
			{
				data.SetBool(IDB_Z ,false);
			}
			if(view==6 || view==7)
			{
				data.SetBool(IDB_Y ,false);
			}

		}
		if(DP.FuzzyPointInRange(data, bd, lineV[0],x,y) && data.GetBool(IDB_X))
		{ 
			EdgeID=100;
		}
		 if(DP.FuzzyPointInRange(data, bd, lineV[1],x,y)&& data.GetBool(IDB_Y))
		{
			EdgeID=200;
		}
		if(DP.FuzzyPointInRange(data, bd, lineV[2],x,y) && data.GetBool(IDB_Z))
		{
			EdgeID=300;
		}

		MMP=bd->SW(Vector(x,y,500));
	}
	else
	{

		CtrlOn = false;
		if (Odlg_flag)
		{
			dlg.Close(false);
			Odlg_flag = 0;
		}
		if (MenuOpen1 == 1)
		{
			Sndlg.Close(false);
			MenuOpen1 = 0;
		}



		if (!AltKey)
		{
			if (!_snap)
			{
				_snap = SnapCore::Alloc();
				if (!_snap) return false;
			}

			// initialize snap always
			if (!_snap->Init(doc, bd))
				return false;


			if (G_STEP == 0)
			{
				//G_markpoint=DP.SnapMouseVector( doc, data, bd, x, y, Vector(), &EndMg, &G_SnapFlag,  G_STEP, &G_P1, &G_P2, &G_nvec,  PlaneSet_BTNFlag,  &FaceMg , &Guide, &_Starget, &SnapID );
				G_markpoint = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_markpoint, G_P1, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
			}
			else if (G_STEP == 1)
			{
				G_P2 = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_P1, G_P2, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
				G_markpoint = G_P2;
				if (G_SnapFlag == 0)
				{
					G_P2 = DP.GetFlatVectorPOS(doc, bd, data, x, y, EndMg, G_STEP, G_markpoint, G_P1, PlaneSet_Flag);
					
				}
				else
				{
					G_P2 = G_markpoint;
				}
			}
			else if (G_STEP == 2)
			{
				G_markpoint = DP.SnapMouseVector(doc, data, bd, _snap, x, y, EndMg, &G_SnapFlag, G_STEP, G_P2, G_P2, &G_nvec, PlaneSet_BTNFlag, &FaceMg, &_MX, &_MY);
				if (G_SnapFlag == 0)
				{
					//DP.printcheck("   --------------G_markpoint=",G_markpoint); 
					G_markpoint = DP.GetFlatVectorPOS(doc, bd, data, x, y, EndMg, G_STEP, G_markpoint, G_P2, PlaneSet_Flag);
				}
			}
			if (qual == QSHIFT)
			{
				ShiftKey = true;
			}
		}
	}

	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
	return true;
	
}

//-------------------------------------------------------------------------------------
//メッセージ
Bool SnapScl::Message(BaseDocument* doc, BaseContainer& data, Int32 type, void* t_data)
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


				if (data.GetInt32(MDATA_SELECTMODE) == CH_EXTUBE)
				{
					if (doc->GetMode() == Medges || doc->GetMode() == Mpolygons)
					{
					}
					else
					{
						if (QuestionDialog("It is not a polygon mode or edge mode.\nAre you sure you want to change the polygon mode?"))
						{
							doc->SetMode(Mpolygons);

						}
						else
						{
							data.SetInt32(MDATA_SELECTMODE, CH_ROT);
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
				EndMg = DP.SetPlaneGetMg(doc, bd, data, PlaneSet_Flag, FaceMg, 2);//　基準平面を設定し最終マトリックスを返す
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
			case MDATA_SAVEB:
					Priinit( data, true );
				break;
			}
		}
		break;
		case MSG_DESCRIPTION_VALIDATE:
			{
				DTOOL DP;
				DP.SetGridLine(doc, data);
				if(!InteractiveValue(doc,data)) return false;
				//GePrint(" MSG_DESCRIPTION_VALIDATE");
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

			//GePrint(" MSG_DESCRIPTION_CHECKUPDATE");
	//**********************************************************************************************		
		}
		break;
	}

	return true;
}



Bool SnapScl::GetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, GeData& t_data, DESCFLAGS_GET& flags)
{
	switch (id[0].id)
	{

	case MDATA_SAVEB:

		flags |= DESCFLAGS_GET_PARAM_GET;
		break;
	}
	return SUPER::GetDParameter(doc, data, id, t_data, flags);
}


//////////////////////////////////////////////////////////////////////
Bool SnapScl::SetDParameter(BaseDocument* doc, BaseContainer& data, const DescID& id, const GeData& t_data, DESCFLAGS_SET& flags)
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
				case MDATA_SAVEB:
				{
					Priinit(data, true);
				}
					break;
			}
		break;


	}

	return SUPER::SetDParameter(doc, data, id, t_data, flags);
}

//////////////////////////////////////////////////////////////////////
//選択ポイント座標を記憶
Bool SnapScl::RectPointVector(BaseObject *op, Int32 i )
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

	mobj[i].Mmg=op->GetMg();

	if(!PO)
	{
		return false;
	}

	BaseSelect *bs=PO->GetPointS();
	Int32 pcnt = bs->GetCount();

	Vector *vp=PO->GetPointW();


	if(op->GetDocument()->GetMode() == Mpoints)
	{
		if(!pcnt)
		{
			return false;
		}

   		if(pcnt)
		{
			mobj[i].scnt=pcnt;
			//mobj[i].vv=(Vector *) GeAlloc(pcnt*sizeof(Vector));
			mobj[i].vv=NewMemClear(Vector, pcnt);


			if(!mobj[i].vv)
			{
			//	DeleteMem(mobj[i].vv);
				return false;
			}
			//mobj[i].pnum=(Int32 *) GeAlloc(pcnt*sizeof(Int32));
			mobj[i].pnum = NewMemClear(Int32, pcnt);

			if(!mobj[i].pnum)
			{
			//	DeleteMem(mobj[i].pnum);
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

	}
	else
	{
		pcnt=PO->GetPointCount();

 		if(!pcnt)
		{
			return false;
		}
     	if(pcnt)
		{
			mobj[i].scnt=pcnt;
			//mobj[i].vv=(Vector *) GeAlloc(pcnt*sizeof(Vector));
			mobj[i].vv=NewMemClear(Vector, pcnt);


			if(!mobj[i].vv)
			{
			//	DeleteMem(mobj[i].vv);
				return false;
			}
			//mobj[i].pnum=(Int32 *) GeAlloc(pcnt*sizeof(Int32));
			mobj[i].pnum = NewMemClear(Int32, pcnt);

			if(!mobj[i].pnum)
			{
			//	DeleteMem(mobj[i].pnum);
				DeleteMem(mobj[i].vv);
				return false;
			}

			for(Int32 j=0; j< pcnt; j++)
			{
				mobj[i].vv[j]=vp[j]; //頂点ベクトルの記憶
				mobj[i].pnum[j]=j;	//ﾎﾟｲﾝﾄ番号の記憶
			
			}
		}


	}
	


	if(op->GetType() == Opolygon)
	{
		
		if(!GetSELPoly(op,  i, 0))
		{
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}
		if(!GetSELEdge(op,  i, 0))
		{
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
//選択ポイントをスケール
Bool SnapScl::SetPointScale( BaseObject *op, Int32 i, Vector Scl, Matrix A_mg )
{
	DTOOL DP;
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


    Int32 Scnt=mobj[i].scnt;
    if(!Scnt) return false;

	//DP.printcheck("------Scl=",Scl);
	//基準平面内のスケールマトリックスを作成
	Matrix mr =MatrixScale(Scl);

    if(Scnt)
    {
		Matrix omg = mobj[i].Mmg; //グローバルマトリックス取得

        if(op->GetType() == Ospline || op->GetType() == Offd)
        {
            Vector* vv=point->GetPointW();
            for(Int32 j=0; j<Scnt; j++)
            {
                Vector v=mobj[i].vv[j]; //ポイント座標取得
				Vector v1 = omg * v;	 //グローバル変換
			//	DP.printcheck("  v1=", v1);
				Vector v2 = v1 - G_P1;	 //１点目からの差を求める
				v2=~A_mg*v2;   //基準平面にローカル変換
			//	DP.printcheck("  v2=", v2);
				v2 = mr*v2;	 //スケール変換
				v2=A_mg*v2;	//ワールド基準に変換
	//			DP.printcheck("scale v2=", v2);
				Vector v3 = v2 + G_P1;	// １点目の座標を足してグローバル座標を求める
			//	DP.printcheck("   Global  v3=", v3);
				v3 = ~omg * v3;	 //オブジェクト内座標に変換
				
                vv[mobj[i].pnum[j]]=v3;
            }
            op->Message(MSG_UPDATE);
        }
        else if(op->GetType() == Opolygon)
        {
            AutoAlloc<Modeling> mod;
            if (!mod || !mod->InitObject(op)) return false;
            for(Int32 j=0; j<Scnt; j++)
            {
                Vector v=mobj[i].vv[j]; //ポイント座標取得
			//	DP.printcheck("v =", v);
				Vector v1 = omg * v;	 //グローバル変換
			//	DP.printcheck("  v1=", v1);
				Vector v2 = v1 - G_P1;	 //１点目からの差を求める
				v2=~A_mg*v2;   //基準平面にローカル変換
			//	DP.printcheck("  v2=", v2);
				v2 = mr*v2;	 //スケール変換
				v2=A_mg*v2;		//ワールド基準に変換
	//			DP.printcheck("scale v2=", v2);
				Vector v3 = v2 + G_P1;	// １点目の座標を足してグローバル座標を求める
				//DP.printcheck("   Global  v3=", v3);
				v3 = ~omg * v3;	 //オブジェクト内座標に変換
			//	DP.printcheck("   v3=", v3);

                if(!mod->SetPoint(op,mobj[i].pnum[j],v3, MODELING_SETPOINT_FLAG_EMPTY)) return false;
            }
            if (!mod->Commit(op, MODELING_COMMIT_UPDATE )) return false;
        }
        //op->Message(MSG_UPDATE);
    }   

    return true;
}

////////////////////////////////////////////////////////////////////
//ポリゴンのポイント座標を記憶
Bool SnapScl::RectPolygonVector(BaseObject *op, Int32 i )
{
	if(op->GetType() != Opolygon ) return false;
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
		//mobj[i].vv=(Vector *) GeAlloc(SelCnt*4*sizeof(Vector));


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
		//	DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}
		//mobj[i].Snum=(Int32 *) GeAlloc(SelCnt*sizeof(Int32));

		mobj[i].Snum=NewMemClear(Int32, SelCnt);
		if(!mobj[i].Snum)
		{
		//	DeleteMem(mobj[i].Snum);
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}

		
		Int32 ndx, seg = 0, smin, smax, i_cnt=0, p=0;
		//選択ﾎﾟｲﾝﾄを格納
		while( PolySel->GetRange(seg++,Poly->GetPolygonCount(),&smin,&smax) )
		{
			for( ndx=smin; ndx<=smax; ndx++ )
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

				mobj[i].Snum[p]=ndx;
				p +=1;
				for(Int32 j=0; j<4; j++)
				{
					mobj[i].vv[i_cnt]=P[j];
					mobj[i].pnum[i_cnt]=num[j];
					i_cnt += 1;
				}
			}
		}
		if(!GetSELEdge(op, i, 2))
		{
			DeleteMem(mobj[i].Snum);
			DeleteMem(mobj[i].pnum);
			DeleteMem(mobj[i].vv);
			return false;
		}
	}

	//ポリゴンIndexをソート
//	dp.QSortPoly(mobj, 0, mobj[i].Fcnt-1, i);
	return true;
}

////////////////////////////////////////////////////////////////////
//エッジのポイント座標を記憶
Bool SnapScl::RectEdgeVector(BaseObject *op, Int32 i )
{
	if(op->GetType() != Opolygon ) return false;
	PointObject *point=ToPoint(op);
	if(!point) return false;
	const Vector *vpoint=point->GetPointR();

	PolygonObject *Pobj=ToPoly(op);	//ポリゴン変換
	const CPolygon*  poly = Pobj->GetPolygonR(); //ポリゴンリスト取得

	BaseSelect *Esel=Pobj->GetEdgeS();

	Int32 ecnt=Esel->GetCount();
	if(!ecnt) return false;
	mobj[i].Ecnt=ecnt;

	//mobj[i].Enum=(Int32 *) GeAlloc(ecnt*sizeof(Int32));
	mobj[i].Enum= NewMemClear( Int32, ecnt);
	if(!mobj[i].Enum)
	{
	//	DeleteMem(mobj[i].Enum);
		return false;
	}

	//Int32 *se=(Int32 *) GeAlloc(ecnt*2*sizeof(Int32));
	Int32 *se= NewMemClear( Int32, ecnt*2);
	if(!se)
	{
	//	DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}

	//Int32 *se2=(Int32 *) GeAlloc(ecnt*2*sizeof(Int32));
	Int32 *se2= NewMemClear( Int32, ecnt*2);
	if(!se2)
	{
	//	DeleteMem(se2);
		DeleteMem(se);
		DeleteMem(mobj[i].Enum);
		return false;
	}
	Int32 ndx, seg = 0, smin, smax, i_cnt=0, ocnt=0, ii=0;
	while( Esel->GetRange(seg++,Pobj->GetPolygonCount()*4,&smin,&smax) )
	{
		for( ndx=smin; ndx<=smax; ndx++ )
		{

			ocnt +=1;
			Int32 polyNdx;
			polyNdx = ndx >> 2; //ポリゴン番号
			mobj[i].Enum[ii]=ndx;
			ii += 1;
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


	//ポリマップ
	//UCHAR* map = NULL;
	UChar* map = NULL;


//	DeleteMem(map);
	//map= (UCHAR*)GeAlloc(i_cnt);
	map = 	NewMemClear(UChar, i_cnt);

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
	//mobj[i].vv=(Vector *) GeAlloc(tcnt*sizeof(Vector));
	mobj[i].vv= NewMemClear(Vector, tcnt);
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
	mobj[i].pnum= NewMemClear(Int32, tcnt);
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
	DeleteMem(map);
	return true;
}

/////////////////////////////////////////////////////////////////
//アンドォ設定
Bool SnapScl::AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE type)
{
	Int32 a;
	for (a = 0; a < arr->GetCount(); a++)
	{
		if (!doc->AddUndo(type, arr->GetIndex(a)))
			return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////
//平面設定
Bool SnapScl::SetPlaneAxis(BaseDocument* doc)
{
	if(!doc) return false;
	ResetTool();
	PlaneSet_BTNFlag=true;


	return true;
}

///////////////////////////////////////////////////////////
//平面リセット
////////////////////////////////////////////////////////
//グリッドリセット
Bool SnapScl::ResetPlaneAxis(BaseDocument* doc, BaseContainer &data)
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

//3pointplane
Bool SnapScl::Set3pointPlane(BaseDocument* doc, BaseContainer &data )
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

///////////////////////////////////////////////////////////
//選択ポリ取得
Bool SnapScl::GetSELPoly(BaseObject *op, Int32 num, Int32 id)
{
	//選択の変換
	BaseContainer bc;
	ModelingCommandData cd;
	BaseDocument *doc=GetActiveDocument();
	cd.doc = doc;
	cd.op = mobj[num].op;
	 
	//選択モードの変換
	bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, id);
	bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,2);
	cd.bc=&bc;
	cd.mode=MODELINGCOMMANDMODE_ALL;
	if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;


	PolygonObject *po=ToPoly(mobj[num].op);
	//pcnt=po->GetPolygonCount();
	BaseSelect *Pbs=po->GetPolygonS();
	Int32 pcnt=Pbs->GetCount();
	//GePrint("Pbs="+String::IntToString(pcnt));
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
		while( Pbs->GetRange(seg++,po->GetPolygonCount(), &smin,&smax) )
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

/////////////////////////////////////////////////////////////////
//選択エッジを取得
Bool SnapScl::GetSELEdge(BaseObject *op, Int32 num, Int32 id)
{
	//選択の変換
		BaseContainer bc;
		ModelingCommandData cd;
		BaseDocument *doc=GetActiveDocument();
		cd.doc = doc;
		cd.op = mobj[num].op;
		bc.SetInt32(MDATA_CONVERTSELECTION_LEFT, id);
		bc.SetInt32(MDATA_CONVERTSELECTION_RIGHT ,1);
		cd.bc=&bc;
		cd.mode=MODELINGCOMMANDMODE_ALL;
		if(!SendModelingCommand(MCOMMAND_CONVERTSELECTION, cd)) return false;
		PolygonObject *po=ToPoly(mobj[num].op);

		BaseSelect *Pbs=po->GetEdgeS();
		Int32 pcnt=Pbs->GetCount();
	
		if(pcnt)
		{
			mobj[num].Ecnt=pcnt;
			mobj[num].Enum= NewMemClear( Int32, pcnt);
			if(!mobj[num].Enum)
			{
			//	DeleteMem(mobj[num].Enum);
				return false;
			}

			Int32 ndx, seg = 0, smin, smax, p=0;
			//選択ﾎﾟｲﾝﾄを格納
			while( Pbs->GetRange(seg++,po->GetPolygonCount()*4, &smin,&smax) )
			{
				for( ndx=smin; ndx<=smax; ndx++ )
				{
					mobj[num].Enum[p]=ndx;
					p +=1;
				}
			}
		}
		return true;
}

Bool SnapScl::InteractiveValue(BaseDocument *doc, BaseContainer &data)
{
	if(!doc) return false;
	if(G_STEP!=2) return false;

	BaseDraw *bbd=doc->GetActiveBaseDraw();
	Int32 view =	bbd->GetProjection();
	Float sx=data.GetFloat(IDSCL_X);
	Float sy=data.GetFloat(IDSCL_Y);
	Float sz=data.GetFloat(IDSCL_Z);

	//ビューによってスケールを設定
	if(view==6 || view==7) //top&bottom
	{
		data.SetFloat( IDSCL_Y, 1);
		sy=1;
		if(!data.GetBool( IDB_X)) sx=1;
		if(!data.GetBool( IDB_Z)) sz=1;
	}
	else if(view==4 || view==5) //front &back
	{
		data.SetFloat( IDSCL_Z, 1);
		sz=1;
		if(!data.GetBool( IDB_X)) sx=1;
		if(!data.GetBool( IDB_Y)) sy=1;
	}
	else if(view==3 || view==2) //right&left
	{
		data.SetFloat( IDSCL_X, 1);
		sx=1;
		if(!data.GetBool( IDB_Y)) sy=1;
		if(!data.GetBool( IDB_Z)) sz=1;
	}
	else
	{
		if(!data.GetBool( IDB_X))
		{
			sx=1;
		}
		if( !data.GetBool( IDB_Y))
		{
			sy=1;
		}
		if( !data.GetBool( IDB_Z))
		{
			sz=1;
		}
	}

	Vector Vsc=Vector( sx,sy,sz);
//	Vector v3=G_markpoint;
	Vector v2=G_P2;
	Vector v1=G_P1;

	Vector L1=v2-v1;
//	Vector L2=v3-v1;
	Matrix Mg= EndMg;
	Mg.off=Vector();

	L1=~Mg*L1; //ローカル化
	Matrix smg=MatrixScale(Vsc);
	L1=smg*L1;
	L1=Mg*L1;
	G_markpoint=G_P1+L1;

	DrawViews(DRAWFLAGS_ONLY_ACTIVE_VIEW | DRAWFLAGS_NO_THREAD|DRAWFLAGS_NO_ANIMATION);
//	EventAdd();
	return true;

}


Bool SnapScl::GetDDescription(BaseDocument* doc, BaseContainer& data, Description* description, DESCFLAGS_DESC& flags)
{
	if (! description->LoadDescription("SnapScale") ) return false;
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

	DescID did3 = DescLevel(MDATA_SAVEB, DTYPE_NONE, 0);
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



	//flags |= DESCFLAGS_DESC_LOADED|DESCFLAGS_DESC_RECURSIONLOCK;
	flags |= DESCFLAGS_DESC_LOADED;
	return ToolData::GetDDescription(doc, data, description, flags);
}





////-------------------------------------------------------------------------------------
//ディスプレイ表示
Bool SnapScl::DispAXIS(BaseDocument *doc, BaseDraw *bd, BaseDrawHelp *bh, BaseContainer &data)
{
	if(!doc) return false;
	bd->SetMatrix_Matrix(nullptr, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(2.0));
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->SetTransparency(0);

	BaseObject* plane = GetWorkplaneObject(doc);

	//ローカルマトリックスに現在の位置を代入しスケールを求める
	Matrix point_mg;
	//point_mg=FaceMg;
	point_mg = plane->GetMg();
	point_mg.off = MemoPos;

	//GePrint("point_mg.x="+FloatToString(point_mg.off.x)+"point_mg.y="+FloatToString(point_mg.off.y)+"point_mg.z="+FloatToString(point_mg.off.z));
	Int32 left, top, right, bottom, width, height;
	Vector ctr = bd->WS(MemoPos);
	
	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Float rad = (Float)((width > height)?(height>>2):(width>>2));
	
      // Scale for PolygonObject
	DTOOL DP;
	Int32 proj =	bd->GetProjection();
	Vector a = DP.ScreenToWorld(ctr, bd, proj);
	Vector b = DP.ScreenToWorld(Vector(ctr.x+rad*0.1, ctr.y, ctr.z), bd, proj);
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
	
	Float len;

	//カラー設定
	Vector Color[3];
	Color[0] = Vector(1,0,0);
	Color[1] = Vector(0,1,0);
	Color[2] = Vector(0,0,1);

	if(CtrlOn)
	{
		BaseDraw *bbd = doc->GetActiveBaseDraw();
		Int32 view =	bbd->GetProjection();
		if(view ==0)
		{
			len=50;
			Vector rot=MatrixToHPB(FaceMg, ROTATIONORDER_DEFAULT);
			point_mg=HPBToMatrix(rot,ROTATIONORDER_DEFAULT);
			point_mg.off=shiftPos;
		}
		else
		{
			len=2.5;
			point_mg.off=shiftPos;
		}

		Aline[0]=Vector(len,0,0);
		Aline[1]=Vector(0,len,0);
		Aline[2]=Vector(0,0,len);


		bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(3.0));
		//XYZのチェック
		if(!data.GetBool(IDB_X))
		{
			Color[0] = Vector(0.3,0,0);
		}
		if(!data.GetBool(IDB_Y))
		{
			Color[1] = Vector(0,0.3,0);
		}
		if(!data.GetBool(IDB_Z))
		{
			Color[2] = Vector(0,0,0.3);
		}
	
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
                    if(data.GetBool(IDB_X ) && data.GetBool(IDB_Y ) )
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
                    
                    if(data.GetBool(IDB_Y ) && data.GetBool(IDB_Z ))
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
                    if(data.GetBool(IDB_Z ) && data.GetBool(IDB_X ) )
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
			//DP.printcheck("ColorV=",ColorV);

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

	}
	else
	{
		len=1.5;
		Aline[0]=Vector(len,0,0);
		Aline[1]=Vector(0,len,0);
		Aline[2]=Vector(0,0,len);
		//軸ライン表示
		for(Int32 i=0; i<3; i++)
		{
			Aline[i]=point_mg*Aline[i];
			bd->SetPen(Color[i]);
			bd->DrawLine(point_mg.off,Aline[i], 0);
		}
	}
	return true;

}



//-------------------------------------------------
//ミニダイアログ設定
//--------------------------------------------------
Bool SnapSCLDataDialog::CreateLayout(void)
{
	GroupBegin(0,BFH_LEFT,0,1,"",0);
	GroupBorderSpace(5, 5, 5, 5);
		AddStaticText(MDATA_TAXUS,BFH_LEFT,15, 10, "", BORDER_NONE);
		AddEditNumberArrows(MDATA_SCL_TEXT,BFH_LEFT, 100, 10);
		AddButton(MDATA_REVERS ,BFH_SCALEFIT,0,5,"F");
		AddButton(MDATA_RESET,BFH_SCALEFIT,0,5,"R");
	GroupEnd();
	return true;
}

Bool SnapSCLDataDialog::InitValues(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_SScale);
	if (!bc) return false;
	Float x=bc->GetFloat(IDSCL_X );
	Float y=bc->GetFloat(IDSCL_Y );
	Float z=bc->GetFloat(IDSCL_Z );
	if(SetFlag==1)
	{
		SetString(MDATA_TAXUS, "X:");
		SetFloat(MDATA_SCL_TEXT,x);
	}
	else if(SetFlag==2)
	{
		SetString(MDATA_TAXUS, "Y:");
		SetFloat(MDATA_SCL_TEXT,y);
	}
	else if(SetFlag==3)
	{
		SetString(MDATA_TAXUS, "Z:");
		SetFloat(MDATA_SCL_TEXT,z);
	}
	return InitDialog();
}

Bool SnapSCLDataDialog::InitDialog(void)
{
	BaseContainer *bc=GetToolData(GetActiveDocument(),ID_SScale);
	if (!bc) return false;

	return true;
}

Bool SnapSCLDataDialog::Command(Int32 id,const BaseContainer &msg)
{
	switch (id)
	{
		case MDATA_SCL_TEXT:
		{
			BaseContainer *bc=GetToolData(GetActiveDocument(),ID_SScale);
			if (!bc) return false;
			Float value;
			GetFloat(MDATA_SCL_TEXT,value);
			if(SetFlag==1)
			{
				bc->SetFloat(IDSCL_X, value);
			}
			else if(SetFlag==2)
			{
				bc->SetFloat(IDSCL_Y, value);
			}
			else if(SetFlag==3)
			{
				bc->SetFloat(IDSCL_Z, value);
			}

			EventAdd();
		}
		break;
		case MDATA_RESET:
		{
			SetInt32(MDATA_SCL_TEXT, 1);

			BaseContainer *bc=GetToolData(GetActiveDocument(),ID_SScale);
			if (!bc) return false;
			if(SetFlag==1)
			{
				bc->SetFloat(IDSCL_X, 1);
			}
			else if(SetFlag==2)
			{
				bc->SetFloat(IDSCL_Y, 1);
			}
			else if(SetFlag==3)
			{
				bc->SetFloat(IDSCL_Z, 1);
			}

			EventAdd();
		}
		break;
		case MDATA_REVERS:
			{
				BaseContainer *bc=GetToolData(GetActiveDocument(),ID_SScale);
				if (!bc) return false;
				Float value;
				GetFloat(MDATA_SCL_TEXT,value);
				value*= -1;
				SetFloat(MDATA_SCL_TEXT,value);
				if(SetFlag==1)
				{
					bc->SetFloat(IDSCL_X, value);
				}
				else if(SetFlag==2)
				{
					bc->SetFloat(IDSCL_Y, value);
				}
				else if(SetFlag==3)
				{
					bc->SetFloat(IDSCL_Z, value);
				}

				EventAdd();
			}
			break;
	}
	return true;
}

Bool SnapSCLDataDialog::SetAdd( Int32 num)
{
	SetFlag=num;
  return true;
}



void SnapScl::Priinit(BaseContainer& data, Bool flag )
{
	String str= "SnS.dat";
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
void SnapScl::ReadData(BaseContainer bc, BaseContainer& data)
{
	data.SetBool(IDB_X, bc.GetBool( IDB_X));
	data.SetBool(IDB_Y, bc.GetBool( IDB_Y));
	data.SetBool(IDB_Z, bc.GetBool( IDB_Z));
	data.SetFloat(IDSCL_X, bc.GetFloat(IDSCL_X) );
	data.SetFloat(IDSCL_Y, bc.GetFloat(IDSCL_Y) );
	data.SetFloat(IDSCL_Z, bc.GetFloat(IDSCL_Z) );
	data.SetInt32(IDG_LOCK, bc.GetInt32(IDG_LOCK) );

	data.SetInt32( IDE_Snap_AXIS, bc.GetInt32( IDE_Snap_AXIS));
	data.SetBool(MDATA_BOOL_GRID_FLAG, bc.GetBool(MDATA_BOOL_GRID_FLAG) );
	data.SetFloat( MDATA_GRID_VALUE, bc.GetFloat(MDATA_GRID_VALUE)  );
	data.SetInt32( MDATA_GRIDLINE, bc.GetInt32( MDATA_GRIDLINE) );
	data.SetInt32( MDATA_LINE, bc.GetInt32(MDATA_LINE)  );
	data.SetInt32( MDATA_GRID_LIST, bc.GetInt32(MDATA_GRID_LIST) );
	data.SetBool(MDATA_VISIBLE, bc.GetBool(MDATA_VISIBLE));
}

//デフォルトデータセット
void SnapScl::DefoultSet( BaseContainer& data)
{

	data.SetBool(IDB_X, true);
	data.SetBool(IDB_Y, true);
	data.SetBool(IDB_Z, true);
	data.SetFloat(IDSCL_X, 1.0);
	data.SetFloat(IDSCL_Y, 1.0);
	data.SetFloat(IDSCL_Z, 1.0);
	data.SetInt32(IDG_LOCK, IDB_LOCKNONE);

	data.SetInt32( IDE_Snap_AXIS, IDE_XZ);
	data.SetBool(MDATA_BOOL_GRID_FLAG, false);
	data.SetFloat( MDATA_GRID_VALUE, 100 );
	data.SetInt32( MDATA_GRIDLINE, 100 );
	data.SetInt32( MDATA_LINE, 10 );
	data.SetInt32( MDATA_GRID_LIST, CH_GRID_L1);
	data.SetBool( MDATA_VISIBLE, false);

}


//パラメータ保存
void SnapScl::SaveData(HyperFile* hf, BaseContainer& data)
{
	BaseContainer bc;

	bc.SetBool(IDB_X, data.GetBool( IDB_X));
	bc.SetBool(IDB_Y, data.GetBool( IDB_Y));
	bc.SetBool(IDB_Z, data.GetBool( IDB_Z));
	bc.SetFloat(IDSCL_X, data.GetFloat(IDSCL_X) );
	bc.SetFloat(IDSCL_Y, data.GetFloat(IDSCL_Y) );
	bc.SetFloat(IDSCL_Z, data.GetFloat(IDSCL_Z) );
	bc.SetInt32(IDG_LOCK, data.GetInt32(IDG_LOCK) );

	bc.SetInt32( IDE_Snap_AXIS, data.GetInt32( IDE_Snap_AXIS));
	bc.SetBool(MDATA_BOOL_GRID_FLAG, data.GetBool(MDATA_BOOL_GRID_FLAG) );
	bc.SetFloat( MDATA_GRID_VALUE, data.GetFloat(MDATA_GRID_VALUE)  );
	bc.SetInt32( MDATA_GRIDLINE, data.GetInt32( MDATA_GRIDLINE) );
	bc.SetInt32( MDATA_LINE, data.GetInt32(MDATA_LINE)  );
	bc.SetInt32( MDATA_GRID_LIST, data.GetInt32(MDATA_GRID_LIST) );
	bc.SetBool(MDATA_VISIBLE, data.GetBool( MDATA_VISIBLE));
	String ver=GeLoadString(IDC_VER);
	bc.SetString( 999,ver);
	hf->WriteContainer(bc);
}


/////////////////////////////////////////////
//プラグインメニュー登録
Bool RegisterSScale()
{
//	String name="SnapScl";
		String name=GeLoadString(IDS_SnapScale);
	//if (!name.Content()) return true;
	//return RegisterToolPlugin(ID_SScale, name, ( PLUGINFLAG_TOOL_TWEAK_NO_HIGHLIGHT | PLUGINFLAG_TOOL_EDITSTATES), AutoBitmap("SnapScale.tif"),GeLoadString( IDS_SnapSC_TOOL_HLP), gNew SnapScl);

	return RegisterToolPlugin(ID_SScale, name, ( PLUGINFLAG_TOOL_TWEAK_NO_HIGHLIGHT | PLUGINFLAG_TOOL_EDITSTATES), AutoBitmap("SnapScale.tif"),GeLoadString( IDS_SnapSC_TOOL_HLP), NewObjClear(SnapScl));


}














