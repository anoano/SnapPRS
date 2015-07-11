#ifndef _DTOOL_H_
#define _DTOOL_H_
#include "c4d_symbols.h"
#include "c4d.h"
#include "lib_collider.h"
#include "lib_clipmap.h"
#include "SnapTransfer.h"
#include "SnapRotate.h"
#include "SnapScale.h"
#include "c4d_descriptiondialog.h"
#include "operatingsystem.h"
#include "lib_splinehelp.h"
#include "lib_selectionchanger.h"
#include "c4d_snapdata.h"
#include "lib_sculpt.h"
  #include "lib_ngon.h"
#include <cmath>
#include "c4d_quaternion.h"
#include "ge_sort.h"
#include "lib_modeling.h"

#define sn_size 16

#define ID_BITMAPBTN_SAVE		22000000


struct Mobj
{
	Vector *vv;	  //ポイント座標
	Matrix Mmg;
	Int32 scnt;  //選択ポイント数
	Int32 *pnum;	//選択ポリゴンを構成するポイントイID用配列
    
	Int32 *Snum;	//選択ポリゴンID格納用配列
	Int32 Fcnt;	   //選択ポリゴン数

	Int32 Ecnt; //選択エッジ数
	Int32 *Enum; //選択エッジID

	UChar* emap;
	Int32 SEcnt;

	BaseObject *op;
};


//////////////////////////////////////////////////////////////////
//クラス定義
class DTOOL 
{
private:

public:
	DTOOL();
//	DTOOL( Matrix *mg, Vector *P1, Vector *P2, Vector *P3, Int32 *STEP, Int32 *SFlag, Vector *nvec, Vector *Snp, Vector *Gmpoint, BaseObject *obj);
	virtual ~DTOOL();

	void printcheck( String st,Vector p);
	void printcheckR( String st,Vector p);
	Bool FuzzyPointInRange(BaseContainer &data, BaseDraw *bd, Vector &p, Float mx = 0, Float my = 0);
	Bool CheckSnpPoint( BaseDraw *bd, BaseContainer &data, BaseObject *obj, Float mx, Float my, Int32 *ID_POINT);
	Bool GetObjectFace(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex, Int32 *PointID );
	BaseObject* GetObjectFaceOBJ(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex,  Bool flag);
	
	
	//Bool SetGridPlane(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, BaseDrawHelp* bh, Matrix mg);
	//Vector GetPointNormalVector( BaseObject *mo ,Int32 index);
	Bool Get3PointNormMg( Vector a, Vector b, Vector c );
	Bool GetLineEdge( BaseDraw *bd, BaseContainer &data, BaseObject *obj, Float mx, Float my, Int32 *index );

	Bool DispMarker(BaseDocument *doc, BaseDraw *bd, BaseDrawHelp *bh, BaseContainer &data, Matrix EndMg, Vector G_markpoint, Bool PlaneSet_Flag, Bool G_SnapFlag, Bool PlaneSet_BTNFlag, Matrix FaceMg, Float mx, Float my, Int32 type);
	Bool DrawTextA(String text,Vector xpos, Float p,  BaseDraw *bd, Int32 EdgeID );
	
	Vector ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection);


	//Vector SnapMouseVector( BaseDocument *doc, BaseContainer &data,BaseDraw *bd, Float x, Float y, Vector G_basePos, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag,  Matrix* SNmg,  BaseObject** _Starget, Int32* SnapID);
	BaseObject* GetObjectFaceID(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex);
	Vector NonObjCursor( BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Float px, Float py, Matrix Amg, Int32 G_STEP, Vector P1, Vector P2, Vector *G_nvec,Bool PL_flag);
	void search_hierarchy(BaseObject *obj1, BaseObject *obj2,AtomArray *arr);

	Vector GetNormlVector(Matrix Mtg);
	Bool ViewSet( BaseDraw *bd,Vector *vm );
	Vector GetZpoint( BaseDocument *doc, BaseContainer &data, BaseDraw *bdd, Vector Nplane, Float *mx, Float *my, Vector Bpos, Int32 step, Vector nov);
	Vector ConstAngleFase( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2,  Float Ang, Matrix mg );
	Vector ConstAngle( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2, Vector v3, Float Ang, Matrix mg );
	Bool SetGridPoint( BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Vector G_point, Matrix mg, Float x, Float y, Int32 G_STEP, Bool PL_flag, Matrix PLmg, Vector* G_markpoint);
	Bool SetGridLine(BaseDocument* doc, BaseContainer& data);
	Bool SP_CrossSnap( BaseDocument *doc, BaseDraw *bd, BaseContainer &data,  Float mx, Float my, Vector *Vspos,  Matrix *Amg );
	Bool CrossSection( Vector As, Vector Ae, Vector Bs, Vector Be, Vector *cross, BaseContainer &data, Float &t1, Float &t2,  Matrix *Amg );
	Bool KeyCoodCheck( Int32 SERIAL_SIZE );
	Bool GetNumber( String &st, Int32 &NUM);
	String GetPSN(const Int32 csn);
	String ExchangeKye(Int32 num);
	Vector GetDrawVec( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2, Matrix mg );
	String geneSN( Int32 sn, Float ch, String s1, String s2);
	Float VectorToRad(Vector v1, Vector v2, Matrix mg);
	Bool CheckSnapSetting( BaseDocument* doc, BaseContainer& data);
	void SetCHECKUP(BaseDocument* doc, BaseContainer& data, Bool flag);

	//void SetObjectPlane(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool flag, Matrix* Amg);
	void init(BaseDocument* doc, BaseContainer& data);


	//ポイントIDから法線ベクトルを返す
	Vector GetVertexNormal(Int32 PointIndex, PolygonObject *op, Neighbor *neighbor);

	Vector GetScaleForMatrix(Matrix mg);

	Float Vector2AngleLocal(Vector vv1, Vector vv2, Matrix mg, Int32 &flag);

	BaseObject* GetTargetObj(BaseDocument* doc, BaseObject* TGobj);
	Vector LockAngle( BaseDocument *doc, BaseContainer &data, Vector v1, Float Ang, Vector GP1, Vector GP2,Matrix Amg);

	Bool Snap90Plane( BaseDocument *doc,BaseContainer &data, BaseDraw *bd,BaseDrawHelp *bh, Vector G_markpoint,  Matrix SNmg  );
	void search_hierarchyDEL(BaseObject *obj1, BaseObject *obj2);
	void QSortK( Mobj *x, Int32 left, Int32 right);
	void SwapK( Mobj *x, Int32 i, Int32 j);
    Bool CrossSectionB( Vector As, Vector Ae, Vector Bs, Vector Be, Vector *crossA, Vector *crossB, BaseContainer &data, Matrix Amg );
    Float firstPoint(Float *Gpos, Float A1, Float A2, Float A3, Float A4, Float A6, Float A7, Float A8, Float A9, Float A10);

	Vector SnapEdgeVector( BaseDocument *doc, BaseContainer &data,BaseDraw *bd, Float x, Float y, Vector G_basePos, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Matrix* SNmg, Vector* va, Vector* vb);
   
    void GetLayerListdata(LayerObject *layer, Bool* flag, LayerObject ** Slayer);

	void SerchiOBJ(BaseObject* op, BaseObject** tg);


	Bool PointerToAXISEdgeCheck( BaseDraw* bd,BaseContainer &data, Float x, Float y, Vector* AP, Vector center, Int32* Num);


	Bool ClingCheck(BaseDocument* doc, BaseObject* op, Int32 id);

	//スナップ先エレメント情報の取得
	Bool GetPolyErementInfo(BaseDocument* doc, BaseDraw *bd, BaseObject* getOBJ, SnapResult Snapres, Int32* G_SnapFlag, Float x, Float y, Matrix* G_mg);
	Vector GetFlatVectorPOS(BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Float x, Float y, Matrix G_mg, Int32 G_STEP, Vector G_markpoint, Vector P1, Bool PL_flag);
	Vector GetZpointNew(BaseDocument *doc, BaseContainer &data, BaseDraw *bdd, Vector Nplane, Float mx, Float my, Vector Bpos, Int32 step, Vector nov);
	Bool GetSnapFaceMatrix(BaseDocument* doc, BaseDraw* bd, Matrix* AxMg, Int32 id, BaseObject* op);
	Vector GetFaceNormal(Vector *vv, CPolygon *pArr, Int32 index);
	Vector GetFaceCenterPos(Vector *vv, CPolygon *pArr, Int32 index);

	Bool  Get3PointNormMg(Vector a, Vector b, Vector c, Matrix* MMG);

	void printcheckS(String st, Matrix mg);
	Matrix GetPlaneMatrix(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool PlaneSet_Flag, Matrix mg);
	//void SetObjectPlane2(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool Pflag, Matrix Amg);
	//*************************************************************
	Bool  SnapSync(BaseDocument* doc, BaseContainer& data, Bool PlaneSet_BTNFlag);

	BaseObject* SnapMouseVectorObj(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, SnapCore*_snap, Float x, Float y, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag, Matrix* ViewMg, Float *mx, Float *my,  Int32* SnapID, Vector& pos);
    
    Bool ResetPlaneAxis(BaseDocument* doc);


	Matrix SetPlaneGetMg(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool Pflag, Matrix FaceMg,  Int32 num);

	Vector SnapMouseVector(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, SnapCore*_snap, Float x, Float y, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag, Matrix* ViewMg, Float *mx, Float *my);
	Vector ReMgVector(BaseDraw* bd,  Matrix mg , Vector pos);

};

#endif // _DTOOL_H_