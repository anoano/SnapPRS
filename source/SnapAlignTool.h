#ifndef _SNAPALIGNTOOL_H_
#define _SNAPALIGNTOOL_H_
#include "c4d.h"
#include "c4d_symbols.h"
#include "lib_modeling.h"

#include "c4d_descriptiondialog.h"
//#include "SnapAlign.h"
#include "Dtool.h"
//#include "lib_snapping.h"
#include "c4d_string.h"
#include "toolclone.h"

#include "c4d_basedraw.h"
#define ID_MODELING_SnapALIGN		1024956

#define SERIAL_SIZE				 12
///////////////////////////////////////////////////////////////////////////////////
//構造体

class SnapTranceToolDialog: public GeDialog
{
		
	public:
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);

		virtual Bool InitDialog(void);
		virtual Bool Command(Int32 id,const BaseContainer &msg);
		virtual Bool AskClose();
		virtual void DestroyWindow();
		Bool EnableGadgets(void);


};



class SnapToolDialog: public GeDialog
{
	private:
		Bool* checkFlag;

	public:
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);

		virtual Bool InitDialog(void);
		virtual Bool Command(Int32 id,const BaseContainer &msg);
		virtual Bool AskClose();
		virtual void DestroyWindow();
		void SetFlag(Bool* flag);
		void EnableGadgets(void);
};


class OffsetDialog: public GeModalDialog
{
	Int32 SetFlag;
	public:
		virtual Bool CreateLayout(void);
		virtual Bool InitValues(void);

		virtual Bool InitDialog(void);
		virtual Bool Command(Int32 id,const BaseContainer &msg);
	virtual Bool AskClose();
	virtual void DestroyWindow();
	virtual Bool RestoreLayout(void* secret);

	Bool SetAdd( Int32 num);
};
class SnapAlign : public DescriptionToolData
{

		//Matrix EndMg;

		Int32 NextFlag;
		Vector G_markpoint;
		Vector G_P1;
		Int32 G_SnapFlag;
		Int32 ObjCnt;
		Mobj *mobj;
		//maxon::BaseArray<Mobj> mobj;
		Vector LocV;
		Bool AltKey;

		Int32 ID_POLY;
		Int32 ID_POINT;
		Bool UV_Sw;
		Bool Drag;
		Vector Mp1;
		Vector MMP;

		Vector AxisPos; //Axis用座標
		Bool CtrlOn; //Axis描画用フラグ

		Vector lineV[3];
		Int32 EdgeID;
		SnapTranceToolDialog dlg;
		SnapToolDialog Sndlg;
		Int32 majer;
		Bool PShift;
		Bool PopFlag;


		
		Bool PlaneSet_BTNFlag;
		Bool PlaneSet_Flag;
		Int32 PlaneSetCnt;
		Vector PlV[3];

		Int32 MenuDrag;
		Vector MenuV1;
		Int32 MenuOpen1;

		OffsetDialog Odlg;
		Int32 Odlg_flag;

		Bool SnpDialogFlag;

		

		Int32 CutSnapFlag; //分割指定モード 用フラグ
        Float MpRX[2];

		Vector SPANV[2];
		Bool ExFlag;

		//メジャー用ターゲットオブジェクト
		BaseObject* _Starget1; 
		BaseObject* _Starget2;

		Int32 SnapID1;
		Int32 SnapID2;
		Vector Get2;
		Float HUDOffset;

		Vector $VectorXYZ[3];

		Vector Aline[3];

		Bool AxisOnFlag; //Ctrlキー軸モード初期化フラグ

		Bool checkFlag;

		Vector MemoPos;	 //Ctrlキーを押した時のマウス座標用変数

		Float _MX; //マウスカーソル座標(２D)
		Float _MY; //マウスカーソル座標(２D)


		Bool InitFlag; //起動時に一度だけmainスナップ設定を行うフラグ


		Matrix FaceMg;		//ポリゴン法線マトリックス
 		Matrix EndMg;		//最終マトリックス



		Bool AddUndo(BaseDocument* doc, AtomArray* arr, UNDOTYPE  type);

		Bool ResetPlaneAxis(BaseDocument* doc, BaseContainer &data);
	
		Vector ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection);
		Bool RectPointVector(BaseDocument *doc, BaseObject *op, Int32 i );
		Bool SetAlign(BaseDocument *doc, BaseContainer &data);
		Bool GetModelInfo(BaseDocument *doc);
		Bool PointCenter(BaseDocument *doc);
		Bool SetOffsetMove(BaseDocument *doc, BaseContainer &data);
		Bool SetUVPointFix( BaseDocument *doc, BaseObject *op,UVWTag *m_uvTag);
		Vector GetUVvectorID( BaseDocument *doc, BaseObject *op,UVWTag *m_uvTag);
		Bool VReset(BaseContainer& data);
		Bool Set3pointPlane(BaseDocument* doc, BaseContainer &data );
		Bool CheckSnapSetting( BaseDocument* doc, BaseContainer& data);
		Bool SetVectorLen(BaseDocument* doc, BaseContainer& data, Vector P1, Vector P2);

		Bool AddDrawViewPoint(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh);

		void ResetTool();
		//Bool SnapSync(BaseDocument* doc, BaseContainer& data);
		void Priinit(BaseContainer& data, Bool flag );

		void SaveData(HyperFile* hf, BaseContainer& data);
		void DefoultSet( BaseContainer& data);
		void ReadData(BaseContainer bc, BaseContainer& data);

		Int32 CheckNgonID(BaseObject* op, Int32 num, Int32 Cnt, Int32 k, Char* map, Vector v, Int32 PoCnt,  Int32 selcn);
		//Int32 CheckNgonID(BaseObject* op, Int32 num, Int32 Cnt, Int32 k,   Vector v ,Int32 PoCnt);
		Bool HikakuShape(const Vector* vv, const CPolygon* polyP,  Int32 AAa, Int32 Ab, Vector v);
		Bool AddDrawEdgeExtension(BaseDocument* doc, BaseContainer& data, BaseDraw* bd, BaseDrawHelp* bh);
		Bool ActionPolyMove(BaseDocument* doc, BaseContainer& data,  Mobj Amobj, Vector vc);
		Bool ActionPolyCutMove(BaseDocument* doc, BaseContainer& data, Mobj Amobj, Vector vc);
		BaseTag*	GetLastTag(BaseDocument* doc, BaseObject* op);
		Bool  DispAXIS(BaseDocument *doc, BaseDraw *bd, BaseDrawHelp *bh, BaseContainer &data);
	public:
		SnapAlign();
		~SnapAlign();
		virtual Int32 GetToolPluginId() { return ID_MODELING_SnapALIGN; }
		virtual const String GetResourceSymbol() { return String("SnapTransfer"); }
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



	virtual Bool CreatePopupMenu(BaseContainer& menu,BaseContainer &data);

	void TEST( BaseObject* obj);

	private:

		SnapCore* _snap;			// the snap object for this tool

protected:

//		DynVectorArray cutpoints;

};




#endif // _DPTOOL_H_