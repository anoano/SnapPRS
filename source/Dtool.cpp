//////////////////////////////////////////////////
//2009.12.8		v1.01	�������[�G���[����
#include "Dtool.h"


//�R���X�g���N�^
DTOOL ::DTOOL ()
{
	//_snap = nullptr;
}
//�f�X�g���N�^
DTOOL ::~DTOOL ()
{
	//if (_snap)
	//	SnapCore::Free(_snap);
	//_snap = nullptr;
}


//�x�N�g���\��
void DTOOL::printcheck(String st, Vector p)
{
	GePrint(st+".x="+String::FloatToString(p.x)+st+".y="+String::FloatToString(p.y)+st+".z="+String::FloatToString(p.z));
}

//�x�N�g���\��
void DTOOL::printcheckR(String st, Vector p)
{
	GePrint(st+".x="+String::FloatToString(Deg(p.x))+" "+st+".y="+String::FloatToString(Deg(p.y))+" "+st+".z="+String::FloatToString(Deg(p.z)));
}

//�x�N�g���\��
void DTOOL::printcheckS(String st, Matrix mg)
{
	Vector v1 = mg.v1;
	Vector v2 = mg.v2;
	Vector v3 = mg.v3;
	GePrint(st + " Scale.x=" + String::FloatToString(Len(v1)) +  " Scale.y=" + String::FloatToString(Len(v2)) +  " Scale.z=" + String::FloatToString(Len(v3)));
}
//*********************************************************************
//�G�b�W�X�i�b�v
//*********************************************************************
Bool  DTOOL ::GetLineEdge( BaseDraw *bd, BaseContainer &data, BaseObject *obj, Float mx, Float my, Int32 *index )
{
	if(!obj) return false;
	PointObject *point=NULL;

	if(obj->GetType() != Opolygon )
	{
		point=(PointObject*)obj->GetCache();
		if(obj->GetType() == Ospline)
		{
			point=ToSpline(obj);
		}
		if(!point)
		{
			point=ToPoint(obj);
		}
	}
	else
	{
		point=ToPoint(obj);
	}
	if(!obj) return false;
	AutoAlloc<ViewportSelect> vps;
	if(!vps) return false;

	Int32 left, top, right, bottom, width, height;
	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Int32 Lx = (Int32)mx;
	Int32 Ly = (Int32)my;
	Int32 rad=data.GetInt32(SNAP_RADIUSG); //����͈�

	if(!vps->Init(width, height, bd, obj,Mpolyedgepoint, true, VIEWPORTSELECTFLAGS_0)) return false;
	vps->SetBrushRadius(rad);
	ViewportPixel* ep=vps-> GetNearestEdge(obj, Lx, Ly, rad);
	
	if(ep)
	{
//		Int32  Id_e;
		//�G�b�W�C���f�b�N�X���擾
		//Id_e = ep->i;
       // GePrint("Id_e ="+String::IntToString(Id_e ));
		//index=Id_e;
		
		vps->ClearPixelInfo((Int32)mx, (Int32)my, VIEWPORT_CLEAR_EDGE);
		return true;
	}


	vps->ClearPixelInfo((Int32)mx, (Int32)my, VIEWPORT_CLEAR_EDGE);
	return false;
}


//*********************************************************************
//�}�E�X�|�C���^��̃I�u�W�F�N�g���擾
//*********************************************************************
Bool DTOOL ::GetObjectFace(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex, Int32 *PointID )
{

	AutoAlloc<C4DObjectList> objList;
	if(!objList) return false;
//	objList->AddObject(op,-5000,0);

	if(!SelectionListCreate(doc, NULL, bd, x, y, NULL, objList)) return false;
	Int32 cnt = objList->GetCount();
	if(cnt)
	{
		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *Getmdl = objList->GetObject(i);
			if(!(Getmdl->GetInfo() & OBJECT_MODIFIER) )
			{
				Int32 id=objList->GetPolyIdx(i);
				*Pindex=id;
				CheckSnpPoint( bd, data, Getmdl, x, y, PointID);
				return true;
			}
		}
	}
	return NULL;
}

//*********************************************************************
//�}�E�X�|�C���^��̃I�u�W�F�N�g���擾
//*********************************************************************
BaseObject* DTOOL::GetObjectFaceOBJ(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex, Bool flag)
{

	AutoAlloc<C4DObjectList> objList;
	if(!objList) return NULL;


	AutoAlloc <AtomArray> arr;
	doc->GetActiveObjects(arr, GETACTIVEOBJECTFLAGS_0);
	//GePrint("cnt="+Int32ToString(arr->GetCount()));
	if(!SelectionListCreate(doc, NULL, bd, x, y, NULL, objList)) return NULL;
	Int32 cnt = objList->GetCount();
	if(cnt)
	{
		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *Getmdl = objList->GetObject(i);
			if(Getmdl  )
			{
				if(flag)
				{
					Int32 id=objList->GetPolyIdx(i);
					if(id >=0)
					{
						*Pindex=id;
						return Getmdl;
					}
				}
				else
				{
					Bool Aflag=false;
			
					for(Int32 j=0; j<arr->GetCount(); j++)
					{
						BaseObject* tg=(BaseObject*)arr->GetIndex(j);
						if(tg == Getmdl) Aflag=true;
					}
					if(!Aflag)
					{

						Int32 id=objList->GetPolyIdx(i);
						if(id >=0)
						{
							*Pindex=id;
							return Getmdl;
						}
					}
				}
			}
		}
	}


	return NULL;
}
//*********************************************************************
//�|�C���g�X�i�b�v
//*********************************************************************
Bool DTOOL ::CheckSnpPoint( BaseDraw *bd, BaseContainer &data, BaseObject *obj, Float mx, Float my, Int32 *ID_POINT)
{

	if(!obj) return false;
	PointObject *point=NULL;

	if(obj->GetType() != Opolygon )
	{
		point=(PointObject*)obj->GetCache();
		if(obj->GetType() == Ospline)
		{
			point=ToSpline(obj);
		}
		if(!point)
		{
			point=ToPoint(obj);
		}
	}
	else
	{
		point=ToPoint(obj);
	}

	if(!point) return false;
	AutoAlloc<ViewportSelect> vps;
	if(!vps) return false;
	Int32 left, top, right, bottom, width, height;
	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Int32 Lx = (Int32)mx;
	Int32 Ly = (Int32)my;
	Int32 rad=data.GetInt32(SNAP_RADIUSG);

	if(!vps->Init(width, height, bd, point, Mpoints, true, VIEWPORTSELECTFLAGS_0)) return false;

	vps->SetBrushRadius(rad);

	ViewportPixel* ep=vps-> GetNearestPoint(point, Lx, Ly, rad);;
	if(ep)
	{
		Int32 p;
		p=ep->i;
		Int32 t= *ID_POINT;
		if(p == t)
		{
			//GePrint("i=" + String::IntToString(p));
			vps->ClearPixelInfo((Int32)mx, (Int32)my, VIEWPORT_CLEAR_POINT);
			return true;
		}
		else
		{
			*ID_POINT=-1;
			vps->ClearPixelInfo((Int32)mx, (Int32)my, VIEWPORT_CLEAR_POINT);
			return false;
		}
	}
	*ID_POINT=-1;
	vps->ClearPixelInfo((Int32)mx, (Int32)my, VIEWPORT_CLEAR_POINT);
	return false;
}

//*********************************************************************
//�}�E�X�|�C���^�ƈ����x�N�g�����r������
//*********************************************************************
Bool DTOOL ::FuzzyPointInRange(BaseContainer &data, BaseDraw *bd, Vector &p, Float mx , Float my )
{

	Int32 rad=data.GetInt32(SNAP_RADIUSG);

	for(Int32 i=0; i<2; i++)
	{
		for(Int32 j=0; j<rad; j++)
		{
			Float a=(Float)j;
			if(i)
			{
				a*=-1;
			}
			Float x=mx+a;
			Float y=my+a;
			if (bd->PointInRange(p, (Int32)x, (Int32)my)) return true;
			if (bd->PointInRange(p, (Int32)mx, (Int32)y)) return true;
			if (bd->PointInRange(p, (Int32)x, (Int32)y)) return true;
		}
	}
		return false;
}




//*********************************************************************
//3�_�̖@�������߂�Matrix��Ԃ�
//*********************************************************************
Bool  DTOOL::Get3PointNormMg( Vector a, Vector b, Vector c) 
{
	if(a == NULL) return false;
	if(b == NULL) return false;
	if(c == NULL) return false;
	//if(!c) return false;
	Vector v1=b-a;
	Vector v2=c-a;
	v1.Normalize();
	v2.Normalize();


	//Vector nv = v2 * v1;
	Vector nv =Cross(v1,v2);
	// nv.x=v2.x * v1.x;




	nv=VectorToHPB(nv);
	Matrix mg=HPBToMatrix(nv,ROTATIONORDER_DEFAULT);
	Matrix r=MatrixRotX(Rad(-90.0));
	mg=mg*r;		
	mg.off=a;

	BaseDocument* doc = GetActiveDocument();
	BaseDraw* bd=doc->GetActiveBaseDraw();
	WorkplaneLock( bd, true);
	BaseObject* plane= GetWorkplaneObject( doc);
	plane->SetMg(mg);
	return true;
}

//*********************************************************************
//3�_�̖@�������߂�Matrix��Ԃ�
//*********************************************************************
Bool  DTOOL::Get3PointNormMg(Vector a, Vector b, Vector c, Matrix* MMG)
{
	if (a == NULL) return false;
	if (b == NULL) return false;
	if (c == NULL) return false;
	//if(!c) return false;
	Vector v1 = b - a;
	Vector v2 = c - a;
	v1.Normalize();
	v2.Normalize();


	//Vector nv = v2 * v1;
	Vector nv = Cross(v1, v2);
	// nv.x=v2.x * v1.x;




	nv = VectorToHPB(nv);
	Matrix mg = HPBToMatrix(nv, ROTATIONORDER_DEFAULT);
	Matrix r = MatrixRotX(Rad(-90.0));
	mg = mg*r;
	mg.off = a;

	BaseDocument* doc = GetActiveDocument();
	BaseDraw* bd = doc->GetActiveBaseDraw();
	WorkplaneLock(bd, true);
	BaseObject* plane = GetWorkplaneObject(doc);
	plane->SetMg(mg);
	*MMG = mg;
	return true;
}

//*********************************************************************
//�}�[�J�[�\���ݒ�
//*********************************************************************
Bool DTOOL::DispMarker( BaseDocument *doc,BaseDraw *bd,BaseDrawHelp *bh, BaseContainer &data, Matrix EndMg, Vector G_markpoint, Bool PlaneSet_Flag, Bool G_SnapFlag, Bool PlaneSet_BTNFlag, Matrix FaceMg, Float mx, Float my, Int32 type  )
{
	if (!doc) return false;


	//Vector ar = MatrixToHPB(G_mg, ROTATIONORDER_DEFAULT);
	//printcheckR("r=", ar);
	//	//bd->SetMatrix_Matrix(NULL, bh->GetMg());
	//�|�C���^��̃��b�h�|�C���g�\��
	bd->SetPen(Vector(1.0, 0.0, 0.0));
	bd->SetMatrix_Matrix(NULL, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_SETZ, DRAW_Z_ALWAYS);
	bd->DrawHandle(G_markpoint, DRAWHANDLE_BIG, NOCLIP_D);
	Int32 view = bd->GetProjection();

	bd->SetPen(Vector(1.0, 0.5, 0.0));



	//���[�J���}�g���b�N�X�Ɍ��݂̈ʒu�������X�P�[�������߂�
	Matrix point_mg;
	point_mg.off = G_markpoint;

	//GePrint("point_mg.x="+FloatToString(point_mg.off.x)+"point_mg.y="+FloatToString(point_mg.off.y)+"point_mg.z="+FloatToString(point_mg.off.z));
	Int32 left, top, right, bottom, width, height;
	Vector ctr = bd->WS(G_markpoint);

	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Float rad = (Float)((width > height) ? (height >> 2) : (width >> 2));

	// Scale for PolygonObject
	Int32 proj = bd->GetProjection();
	Vector a = ScreenToWorld(ctr, bd, proj);
	Vector b = ScreenToWorld(Vector(ctr.x + rad*0.1, ctr.y, ctr.z), bd, proj);
	Int32 rd = data.GetInt32(SNAP_RADIUSG);

	Float rad2 = rd * Len(b - a);
	rad2 *= 0.1;
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

	//�����N�X���b�g�܂��͔C�ӕ��ʗL���̏ꍇ�͎���\��
	if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc) || PlaneSet_Flag)
	{
		BaseObject* plane = GetWorkplaneObject(doc);
		Matrix Apoint_mg = plane->GetMg();


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
		//�����C���̒���
		Vector Aline[3];
		Float len = 1.5;
		Aline[0] = Vector(len, 0, 0);
		Aline[1] = Vector(0, len, 0);
		Aline[2] = Vector(0, 0, len);

		//�J���[�ݒ�
		Vector Color[3];
		Color[0] = Vector(1, 0, 0);
		Color[1] = Vector(0, 1, 0);
		Color[2] = Vector(0, 0, 1);

		Vector Lpos = Vector();

		if (PlaneSet_Flag)
		{
			Lpos = Apoint_mg.off;
		}
		else if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
		{
			Lpos = data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)->GetMg().off;
		}
		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		//�����C���\��
		for (Int32 i = 0; i<3; i++)
		{

			Aline[i] = Apoint_mg*Aline[i];
			bd->SetPen(Color[i]);
			bd->DrawLine(Lpos, Aline[i], NOCLIP_D);

		}
	}

	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));


	/*�J�[�\���\���ݒ�
	  �X�i�b�v���Ă��Ȃ����F
	  �I���\�\���F�~�J�[�\��
	 
	*/
	if (G_SnapFlag == 0)
	{
		//	�I���\�\���� �~�J�[�\��
		if (view > 1 && view < 9 )
		{
			bd->SetPen(Vector(1.0, 0.5, 0.0));
			bd->DrawCircle2D((Int32)mx, (Int32)my, data.GetInt32(SNAP_RADIUSG));
			bd->SetPen(Vector(1.0, 0.0, 0.0));
			bd->DrawHandle2D(Vector(mx, my, 0), DRAWHANDLE_BIG);
		}
		else
		{
			//  �p�[�X�y�N�e�B�u
			if (type == 0)//position�̏ꍇ
			{
				//�~�J�[�\��
				bd->SetPen(Vector(1.0, 0.5, 0.0));
				bd->DrawCircle2D((Int32)mx, (Int32)my, data.GetInt32(SNAP_RADIUSG));
				bd->SetPen(Vector(1.0, 0.0, 0.0));
				bd->DrawHandle2D(Vector(mx, my, 0), DRAWHANDLE_BIG);
				
			}
			else //Rotate�܂���scale�̏ꍇ
			{
				bd->SetPen(Vector(1.0, 0.5, 0.0));
				//�T�[�N������]������ϊ��}�g���b�N�X��p��
				Matrix mgrx;

				Float ra = 90.0;

				mgrx = MatrixRotX(Rad(ra));


				bd->LineZOffset(32);

				//�O���[�o���}�g���b�N�X�����[�J���ɑ�����ʒu�����Z�b�g
				Matrix MMO = EndMg;

				MMO.off = Vector(0, 0, 0);

				point_mg = point_mg * MMO;
				//	//�}�g���b�N�X���Z 90�x��]������
				point_mg = point_mg*mgrx;

				//�}�E�X�ʒu��ݒ�
				point_mg.off = G_markpoint;

				bd->DrawCircle(point_mg); //�T�[�N��

				//���S���C���̒��������߂�
				Vector sv = Vector(0, 0, 1);
				sv = point_mg*sv;
				bd->DrawLine(point_mg.off, sv, NOCLIP_D);
			}
		}


	}
	else if (G_SnapFlag == 2 &&  PlaneSet_BTNFlag)
	{
		if (view > 1 && view < 9  )
		{

		}
		else
		{
			//�p�[�X�y�N�e�B�u�r���[�̂݃|���S���@���ɉ����ăJ�[�\����]
			bd->SetPen(Vector(1.0, 0.5, 0.0));
			//�T�[�N������]������ϊ��}�g���b�N�X��p��
			Matrix mgrx;

			Float ra = 90.0;

			mgrx = MatrixRotX(Rad(ra));


			bd->LineZOffset(32);

			//�O���[�o���}�g���b�N�X�����[�J���ɑ�����ʒu�����Z�b�g
			Matrix MMO = FaceMg;

			MMO.off = Vector(0, 0, 0);

			point_mg = point_mg * MMO;
			//	//�}�g���b�N�X���Z 90�x��]������
			point_mg = point_mg*mgrx;

			//�}�E�X�ʒu��ݒ�
			point_mg.off = G_markpoint;

			bd->DrawCircle(point_mg); //�T�[�N��

			//���S���C���̒��������߂�
			Vector sv = Vector(0, 0, 1);
			sv = point_mg*sv;
			bd->DrawLine(point_mg.off, sv, NOCLIP_D);

		}
	}

	
	return true;
}


//*********************************************************************
//�e�L�X�g�\��
//*********************************************************************
Bool DTOOL::DrawTextA(String text,Vector xpos, Float p,  BaseDraw *bd, Int32 EdgeID )
{
	AutoAlloc<GeClipMap> cm;
	if(!cm) return false;
	if(cm->Init(0, 0, 32) != IMAGERESULT_OK) return false;
	cm->BeginDraw();

	Float scale = 1.5;
	Int32 width = cm->GetTextWidth(text);
	Int32 height = cm->GetTextHeight();
	width = (Int32)(width*1.2);
	height += 5;

	width = (Int32)(width* scale);
	height = (Int32)(height* scale);

	cm->EndDraw();
	cm->Destroy();

	if(cm->Init(width, height, 32) != IMAGERESULT_OK) return false;
	 cm->BeginDraw();


    BaseContainer bc;
    if (!cm->GetDefaultFont(GE_FONT_DEFAULT_SYSTEM, &bc)) return false;

	Float size;
	if(cm->GetFontSize(&bc,GE_FONT_SIZE_INTERNAL, &size) ) 
	{
		size *= scale;
		if(!cm->SetFont( &bc, size)) return false;
	}

	 if(EdgeID == 1 )
	 {
		cm->SetColor(255, 255, 0, 255);
	 }
	 else if(EdgeID == 999 )
	 {
		cm->SetColor(255, 255, 255, 255);
	 }
	 else
	 {
		cm->SetColor(255, 250, 0, 255);
	 }


	
	#ifdef __PC
		{
			cm->TextAt(5,2,text);
		}
	#else
		{
			cm->TextAt(0,0,text);
		}
	#endif

	cm->EndDraw();

	bd->SetMatrix_Screen();
	bd->SetLightList(BDRAW_SETLIGHTLIST_NOLIGHTS);


	//Vector *padr = new Vector[4];
	//Vector *cadr = new Vector[4];
	//Vector *vnadr = new Vector[4];
	//Vector *uvadr = new Vector[4];

	//maxon::BaseArray<Vector*>padr;
	Vector *padr = NewMemClear(Vector, 4);

	Vector *cadr = NewMemClear(Vector, 4);
	Vector *vnadr = NewMemClear(Vector, 4);
	Vector *uvadr = NewMemClear(Vector, 4);


	Vector xp;
	xp=bd->WS(xpos);

	Float xs, ys;
	//xs=xp.x-10;
	//ys=xp.y-p;
	xs=xp.x;
	ys=xp.y-p;
	padr[0] = Vector(xs,ys,0);
	padr[1] = Vector(xs+width,ys,0);
	padr[2] = Vector(xs+width,ys+height,0);
	padr[3] = Vector(xs,ys+height,0);

	cadr[0] = Vector(1,1,1);
	cadr[1] = Vector(1,1,1);
	cadr[2] = Vector(1,1,1);
	cadr[3] = Vector(1,1,1);

	vnadr[0] = Vector(0,0,1);
	vnadr[1] = Vector(0,0,1);
	vnadr[2] = Vector(0,0,1);
	vnadr[3] = Vector(0,0,1);

	uvadr[0] = Vector(0,0,0);
	uvadr[1] = Vector(1,0,0);
	uvadr[2] = Vector(1,1,0);
	uvadr[3] = Vector(0,1,0);
	BaseBitmap *cmbmp = NULL;
	cmbmp = cm->GetBitmap();
	if(!cmbmp) return false;

	BaseBitmap *bmp = NULL;
	bmp = cmbmp->GetClone();
	if(!bmp) return false;

	BaseBitmap *alpha = NULL;
	alpha = bmp->GetInternalChannel();
	if(!alpha) alpha = bmp->AddChannel(true, false);
	if(!alpha)
	{
	  BaseBitmap::Free(bmp);
	  return false;
	}
	Int32 x,y;
	for(y=0; y<height; y++)
	{
	  for(x=0; x<width; x++)
	  {

		  // bmp->SetAlphaPixel(alpha, x, y, 255); //r is the opacity
		   UInt16 r;
		   bmp->GetPixel(x,y,&r,&r,&r);
		   // GePrint("r="+Int32ToString((Int32)r));
		   if(r > 50)
		   {
				bmp->SetAlphaPixel(alpha, x, y, 255); //r is the opacity
		   }
		   else
		   {
				bmp->SetAlphaPixel(alpha, x, y, 125); //r is the opacity
		   }
	  }
	}


	bd->DrawTexture(bmp, padr, cadr, vnadr, uvadr, 4, DRAW_ALPHA_NORMAL, DRAW_TEXTUREFLAGS_0);

	BaseBitmap::Free(bmp);  

	//bDelete(padr);
	//bDelete(cadr);
	//bDelete(vnadr);
	//bDelete(uvadr);

	DeleteMem(padr);
	DeleteMem(cadr);
	DeleteMem(vnadr);
	DeleteMem(uvadr);

	bd->SetDepth(true); 
	return true;
}

//*********************************************************************
//�X�N���[�����W���烏�[���h���W�ɕϊ�
//*********************************************************************
Vector DTOOL::ScreenToWorld(const Vector& pp, BaseDraw* bd, Int32 projection)
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



////*********************************************************************
////�}�E�X�|�C���g�̃��[���h�ʒu�����擾
////*********************************************************************
//Vector DTOOL::SnapMouseVector( BaseDocument *doc, BaseContainer &data,BaseDraw *bd, Float x, Float y, Vector G_basePos, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag, Matrix* SNmg,  BaseObject** _Starget, Int32* SnapID)
//{
//	Vector MV = bd->SW(Vector(x, y, 500));
//	if (!doc) return MV;
//
//
//	//�X�i�b�v�L��
//	if (IsSnapEnabled(doc))
//	{
//
//		if (doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
//		{
//			MV = bd->SW(Vector(x, y, -100000));
//			//GePrint("jjjjJ");
//		}
//
//		//�V�F�[�f�B���O���[�h�̎擾
//		GeData d;
//
//		//	GeDaGeData d;
//		bd->GetParameter(BASEDRAW_DATA_SDISPLAYACTIVE, d, DESCFLAGS_GET_0);
//
//		//6:line���[�h
//
//		AutoAlloc<SnapCore> snap;
//		if (!snap) return MV;;
//
//		if (!snap->Init(doc, bd)) return MV;
//
//		Int32 view = NULL;
//		view = bd->GetProjection();
//		SnapResult Snapres;
//
//		//Vector nv=Vector(0,1,0);
//		SNAPFLAGS sn = SNAPFLAGS_0;
//
//		if (G_STEP > 1)
//		{
//			sn = SNAPFLAGS_IGNORE_SELECTED;
//		}
//
//		
//
//		Bool checkFlag = true;
//
//
//
//		if (snap->Snap(MV, Snapres, sn))
//		{
//
//			if (Snapres.snapmode == SNAPMODE_POINT && d.GetInt32() != 6 && data.GetBool(MDATA_VISIBLE))
//			{
//
//
//				if (!CheckSnpPoint(bd, data, (BaseObject*)Snapres.target, x, y, SnapID))
//				{
//					checkFlag = false;
//					*G_SnapFlag = 0;
//				}
//			}
//			else if (Snapres.snapmode == SNAPMODE_EDGE && d.GetInt32() != 6 && data.GetBool(MDATA_VISIBLE))
//			{
//				Int32 s;
//				if (!GetLineEdge(bd, data, (BaseObject*)Snapres.target, x, y, &s))
//				{
//					checkFlag = false;
//					*G_SnapFlag = 0;
//				}
//
//			}
//			if (checkFlag)
//			{
//
//				// GePrint("aaaa");
//				MV += Snapres.delta; //if pos can be snapped it is set accordingly
//
//				//�X�i�b�v�^�[�Q�b�g�̏����擾
//				*_Starget = (BaseObject*)Snapres.target;
//
//				*SnapID = Snapres.component;
//
//
//				//�I���\�r���[���͂QD�X�i�b�v���W���擾
//				if (doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
//				{
//					MV = Snapres.initial_snap;
//				}
//
//
//				if (IsSnapEnabled(doc, SNAPMODE_DYNAMICGUIDE))
//				{
//					if (Snapres.snapmode == SNAPMODE_DYNAMICGUIDE || Snapres.snapmode == SNAPMODE_ORTHO)
//					{
//						Matrix Smg = Snapres.mat;
//						Smg.off = MV;
//						*SNmg = Smg;
//						//if (Snapres.snapmode == SNAPMODE_ORTHO) *Guide = 2;
//						//else *Guide = 1;
//						//*G_SnapFlag = 1;
//					}
//				}
//
//
//				if (SNAPMODE_WORKPLANE != Snapres.snapmode)*G_SnapFlag = 5;
//				else *G_SnapFlag = 0;
//
//				if (Snapres.snapmode == SNAPMODE_POINT) *G_SnapFlag = 1;
//				else if (Snapres.snapmode == SNAPMODE_POLYGON)*G_SnapFlag = 2;
//				else if (Snapres.snapmode == SNAPMODE_MIDPOINT || Snapres.snapmode == SNAPMODE_POLYGONCENTER)*G_SnapFlag = 3;
//				else if (Snapres.snapmode == SNAPMODE_EDGE)*G_SnapFlag = 4;
//				if ((PlaneSet_Flag && (Snapres.snapmode == SNAPMODE_POLYGON || Snapres.snapmode == SNAPMODE_POLYGONCENTER)) || data.GetBool(MDATA_NORMALPOSITION))
//				{
//
//					//	Int32 PolyID=NULL;
//					//BaseObject *getOBJ = GetObjectFaceOBJ(doc, bd,data, x, y, &PolyID, PlaneSet_Flag);
//					BaseObject *getOBJ = (BaseObject*)Snapres.target;
//
//					if (getOBJ)
//					{
//						BaseObject* res = NULL;
//						//�X�J���v�g�̃`�F�b�N
//						if (getOBJ->GetTag(Tsculpt))
//						{
//							SculptTag* sctag = (SculptTag*)getOBJ->GetTag(Tsculpt);
//							SculptObject* Sc = sctag->GetSculptObject();
//							if (Sc->IsFrozen())
//							{
//								Sc->SetFrozen(false);
//							}
//							Int32 sub = Sc->GetSubdivisionCount();
//							//Fix���ꂽ�|���S�����擾
//							PolygonObject* poly = Sc->GetPolygonCopy(sub, true); //�|���S�����R�s�[
//							res = (BaseObject*)poly;
//						}
//						else
//						{
//							res = GetTargetObj(doc, getOBJ);
//						}
//						if (!res) return MV;
//						PolygonObject *Ppoly = NULL;
//						Ppoly = ToPoly(res);
//						if (Ppoly && Ppoly->GetPolygonCount() >0) //�f�t�H�[�}�[�`�F�b�N
//						{
//							if (Snapres.snapmode == SNAPMODE_POLYGON || Snapres.snapmode == SNAPMODE_POLYGONCENTER)
//							{
//								//�R���W��������
//								AutoAlloc<GeRayCollider> rc;
//								if (!rc)
//								{
//									BaseObject::Free(res);
//									getOBJ = NULL;
//									*G_SnapFlag = 0;
//									return MV;
//								}
//								GeRayColResult Fres;
//								Vector wtail = bd->SW(Vector(x, y, 0));
//								Vector whead = bd->SW(Vector(x, y, 20000000.0));
//								Vector otail = (~res->GetMg()) * wtail;
//
//								//Vector oray = (whead - wtail) ^ (!res->GetMg());
//
//								//Vector oray =  (~res->GetMg()).TransformVector((whead - wtail)) ;
//								//GePrint("jjj");
//								Vector oray = (~res->GetMg()).TransformVector(whead - wtail);
//
//
//
//								rc->Init(Ppoly, true);
//								rc->Intersect(otail, !oray, 20000000.0);
//								if (rc->GetNearestIntersection(&Fres))
//								{
//									Matrix Amg = res->GetMg();
//#ifdef __PC
//									{
//										//  if (dx == 0.0 && dy == 0.0) continue;
//									}
//#else
//									{
//										MV = Amg*Fres.hitpos;
//									}
//#endif                             
//
//									//Vector rv= MatrixToHPB(Amg,ROTATIONORDER_DEFAULT);
//									Amg.off = Vector();
//									Vector cp = Fres.s_normal;//�t�H���O�̖@���x�N�g��
//
//									cp = Amg*cp;
//									cp.Normalize();
//
//
//									Vector Prot = VectorToHPB(cp); //�x�N�g������HPB�ɕϊ�
//									Matrix lp = HPBToMatrix(Prot, ROTATIONORDER_DEFAULT); //HPB����}�g���b�N�X�ϊ�
//									Matrix Mgx = MatrixRotX(Rad(-90.0)); //�X�O�x��]�}�g���b�N�X�쐬
//									Matrix Low_Mg = lp*Mgx;  //�ϊ��}�g���b�N�X�łX�O�x��]
//
//									//Matrix Low_Mg=Mgx * lp;  //�ϊ��}�g���b�N�X�łX�O�x��]
//									//	Matrix Low_Mg=lp;  //�ϊ��}�g���b�N�X�łX�O�x��]
//									//*G_mg = Low_Mg; //�@���}�g���b�N�X��S�̃}�g���b�N�X�ɐݒ�
//									*G_SnapFlag = 2;
//
//								}
//							}
//							else if (Snapres.snapmode == SNAPMODE_POINT)
//							{
//
//								Int32 p = Snapres.component;
//
//								Neighbor nbr;
//								PointObject* Pop = ToPoint(res);
//								if (!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return MV;
//								Vector Pnv = GetVertexNormal(p, Ppoly, &nbr);
//								Vector PR = VectorToHPB(Pnv);
//								Matrix Mgx = MatrixRotX(Rad(-90.0));
//								Matrix lp = HPBToMatrix(PR, ROTATIONORDER_DEFAULT);
//								Matrix Low_Mg = lp*Mgx;
//								//*G_mg = Low_Mg;
//								*G_SnapFlag = 1;
//
//							}
//							else if (Snapres.snapmode == SNAPMODE_EDGE || Snapres.snapmode == SNAPMODE_EDGEMID)
//							{
//
//								const CPolygon*  poly = Ppoly->GetPolygonR(); //�|���S�����X�g�擾
//								Int32 a, b, c, Id_e;
//								a = NULL;
//								b = NULL;
//								c = NULL;
//								//�G�b�W�C���f�b�N�X���擾
//								//	Id_e = ep->i;
//								Id_e = Snapres.component;
//								//�G�b�W�ɂȂ���|�C���g�C���f�b�N�X�擾
//								switch (Id_e % 4)
//								{
//								case 0: a = poly[Id_e / 4].a; b = poly[Id_e / 4].b; c = poly[Id_e / 4].c; break;
//								case 1: a = poly[Id_e / 4].b; b = poly[Id_e / 4].c; c = poly[Id_e / 4].d; break;
//								case 2: a = poly[Id_e / 4].c; b = poly[Id_e / 4].d; c = poly[Id_e / 4].a; break;
//								case 3: a = poly[Id_e / 4].d; b = poly[Id_e / 4].a; c = poly[Id_e / 4].b; break;
//								}
//
//								Neighbor nbr;
//								PointObject* Pop = ToPoint(res);
//								if (!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return MV;
//								Vector PnvA = GetVertexNormal(a, Ppoly, &nbr);
//								Vector PnvB = GetVertexNormal(b, Ppoly, &nbr);
//								Vector Pnv = (PnvA + PnvB) / 2;
//								Vector PR = VectorToHPB(Pnv);
//								Matrix Mgx = MatrixRotX(Rad(-90.0));
//								Matrix lp = HPBToMatrix(PR, ROTATIONORDER_DEFAULT);
//								Matrix Low_Mg = lp*Mgx;
//								//*G_mg = Low_Mg;
//								*G_SnapFlag = 3;
//								//}
//								//vps->ClearPixelInfo(x, y, VIEWPORT_CLEAR_POINT);
//							}
//							//	GePrint("G_SnapFlag ="+Int32ToString(*G_SnapFlag));
//						}
//						else
//						{
//							*G_SnapFlag = 0;
//						}
//						BaseObject::Free(res);
//						res = NULL;
//					}
//					else
//					{
//						*G_SnapFlag = 0;
//					}
//				}
//			}
//
//		}
//		else
//		{
//			*G_SnapFlag = 0;
//		}
//		snap.Free();
//	}
//	else
//	{
//		*G_SnapFlag = 0;
//	}
//
//	//	GePrint("G_SnapFlag ="+Int32ToString(*G_SnapFlag));
//	if (*G_SnapFlag == 0)
//	{
//		MV = NonObjCursor(doc, bd, data, x, y, G_mg, G_STEP, G_markpoint, G_P2, G_nvec, PlaneSet_Flag);
//
//	}
//
//
//	return MV;	
//}
//
//-------------------------------------------------------------------------------
//�}�E�X�|�C���^��̃I�u�W�F�N�g���擾
BaseObject* DTOOL::GetObjectFaceID(BaseDocument *doc, BaseDraw *bd, BaseContainer& data, Float x, Float y ,Int32 *Pindex)
{

	AutoAlloc<C4DObjectList> objList;
    if(!objList) return nullptr;

	if(!SelectionListCreate(doc, NULL, bd, x, y, NULL, objList)) return nullptr;
	Int32 cnt = objList->GetCount();
	if(cnt)
	{
		for(Int32 i=0; i<cnt; i++)
		{
			BaseObject *Getmdl = objList->GetObject(i);
			if(Getmdl  )
			{
				Int32 id=objList->GetPolyIdx(i);
				*Pindex=id;
				return Getmdl;
			}
		}
	}


	return nullptr;
}


//-------------------------------------------------------------------------------------
//�I�����Ă��Ȃ��ꍇ�̈ʒu�ƃ}�g���b�N�X�ݒ�
Vector DTOOL::NonObjCursor( BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Float px, Float py, Matrix Amg, Int32 G_STEP, Vector P1, Vector P2, Vector *G_nvec,Bool PL_flag)
{
	Vector nov = bd->SW(Vector(px,py,500));
	//Vector rmv=NULL;

	if( !G_STEP)
	{
		BaseObject* plane= GetWorkplaneObject( doc);
	
		Vector pos=plane->GetMg().off;

		//��@������
		Vector nv=GetNormlVector(Amg);
		ViewSet(bd, &nv );

		nov=GetZpoint(doc, data, bd, nv, &px, &py, pos, G_STEP, nov);
	}
	else if(G_STEP == 1 || G_STEP == 2 )
	{
		Vector nv=GetNormlVector(Amg);
		ViewSet(bd, &nv );
		nov=GetZpoint(doc, data, bd, nv, &px, &py, P1, G_STEP, nov);
	}
	
	return nov;
}
/////////////////////////////////////////////////////////////////////////////////
//������̍��W��Ԃ�
/////////////////////////////////////////////////////////////////////////////////
Vector DTOOL::GetZpoint(BaseDocument *doc, BaseContainer &data, BaseDraw *bdd, Vector Nplane, Float *mx, Float *my, Vector Bpos, Int32 step, Vector nov)
{
	Vector potZ = nov;
	Vector nearpos = bdd->SW(Vector(*mx, *my, 0));
	Vector farpos = bdd->SW(Vector(*mx, *my, 10000.0));
	Vector ray = farpos - nearpos;
	ray.Normalize();
	if (ray.y <= 0)
	{
		//����_
		Float Lray = Dot(ray, Nplane);

		Float L$p0 = Dot((Bpos - nearpos), Nplane);
		potZ = nearpos + (L$p0 / Lray)*ray;

	}
	else
	{
		if (step == 0 || step == 1 || step == 4)
		{
			Bool flag1, flag2;
			flag1 = flag2 = false;
			if (!IsSnapEnabled(doc, SNAPMODE_WORKPLANE))
			{
				EnableSnap(true, doc, SNAPMODE_WORKPLANE);
				flag1 = true;
			}

			if (!IsSnapEnabled(doc))
			{
				EnableSnap(true, doc);
				flag2 = true;
			}

			AutoAlloc<SnapCore> snap;
			if (!snap) return Vector();
			snap->Init(doc, bdd);
			SnapResult Snapres;
			if (snap->Snap(farpos, Snapres))
			{
				farpos += Snapres.delta; //if pos can be snapped it is set accordingly
				potZ = farpos;
			}
			snap.Free();
			if (flag1) EnableSnap(false, doc, SNAPMODE_WORKPLANE);
			if (flag2) EnableSnap(false, doc);

		}
		else if (step == 2 || step == 5)
		{
			potZ = farpos;
		}

	}
	return potZ;
}


/////////////////////////////////////////////////////////////////////////////////
//�I�����Ă��Ȃ��ꍇ�̈ʒu�ƃ}�g���b�N�X�ݒ�
/////////////////////////////////////////////////////////////////////////////////
Vector DTOOL::GetFlatVectorPOS(BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Float x, Float y, Matrix G_mg, Int32 G_STEP, Vector G_markpoint, Vector P1, Bool PL_flag)
{
	Vector nov = G_markpoint;

	//printcheck(" nov=",nov);
	if (!G_STEP)
	{
		BaseObject* plane = GetWorkplaneObject(doc);
		//*G_mg = plane->GetMg();//�}�g���b�N�X��ݒ�
		Vector pos = plane->GetMg().off;

		//��@������
		Vector nv = GetNormlVector(G_mg);
		ViewSet(bd, &nv);

		nov = GetZpointNew(doc, data, bd, nv, x, y, pos, G_STEP, nov);
	}
	else if (G_STEP == 1 || G_STEP == 2)
	{
		Vector nv = GetNormlVector(G_mg);
		//printcheck(" nv=",nv);
		ViewSet(bd, &nv);
		nov = GetZpointNew(doc, data, bd, nv, x, y, P1, G_STEP, nov);
	}
	//printcheck(" ------nov=",nov);
	return nov;
}



/////////////////////////////////////////////////////////////////////////////////
//������̍��W��Ԃ�
/////////////////////////////////////////////////////////////////////////////////
Vector DTOOL::GetZpointNew(BaseDocument *doc, BaseContainer &data, BaseDraw *bdd, Vector Nplane, Float mx, Float my, Vector Bpos, Int32 step, Vector nov)
{
	Vector potZ = nov;
	Vector nearpos = bdd->SW(Vector(mx, my, 0));
	Vector farpos = bdd->SW(Vector(mx, my, 10000.0));
	Vector ray = farpos - nearpos;
	ray.Normalize();
	if (ray.y <= 0)
	{
		//����_
		Float Lray = Dot(ray, Nplane);

		Float L$p0 = Dot((Bpos - nearpos), Nplane);
		potZ = nearpos + (L$p0 / Lray)*ray;

	}
	else
	{
		
		//if (step == 0 || step == 1 || step == 4)
		//{
		//	GePrint(" GetZpointNew");
		//	Bool flag1, flag2;
		//	flag1 = flag2 = false;
		//	if (!IsSnapEnabled(doc, SNAPMODE_WORKPLANE))
		//	{
		//		EnableSnap(true, doc, SNAPMODE_WORKPLANE);
		//		flag1 = true;
		//	}

		//	if (!IsSnapEnabled(doc))
		//	{
		//		EnableSnap(true, doc);
		//		flag2 = true;
		//	}

		//	AutoAlloc<SnapCore> snap;
		//	if (!snap) return Vector();
		//	snap->Init(doc, bdd);
		//	SnapResult Snapres;
		//	if (snap->Snap(farpos, Snapres))
		//	{
		//		farpos += Snapres.delta; //if pos can be snapped it is set accordingly
		//		potZ = farpos;
		//	}
		//	snap.Free();
		//	if (flag1) EnableSnap(false, doc, SNAPMODE_WORKPLANE);
		//	if (flag2) EnableSnap(false, doc);

		//}
		//else if (step == 2 || step == 5)
		//{
			potZ = farpos;
		//}

	}
	return potZ;
}


void DTOOL::search_hierarchy(BaseObject *obj1, BaseObject *obj2,AtomArray *arr)
{
	while(obj1)
	{
		if(obj1==obj2)return;
		arr->Append(obj1);
		search_hierarchy(obj1->GetDown(),obj2,arr);
		obj1=obj1->GetNext();
	}
}






void DTOOL::search_hierarchyDEL(BaseObject *obj1, BaseObject *obj2)
{
	while(obj1)
	{
		if(obj1==obj2)return;
		//if(obj1->GetEditorMode() != MODE_OFF || obj1->GetRenderMode() != MODE_OFF )
		if(obj1->GetEditorMode() == MODE_OFF  )
		{
			BaseObject* cp=obj1;
			obj1=obj1->GetNext();
			cp->Remove();
		}

		if(obj1)
		{
			search_hierarchyDEL(obj1->GetDown(),obj2);
			obj1=obj1->GetNext();
		}
	}
}
////******************************************************************************
////���_�@�������߂�
////******************************************************************************
//Vector DTOOL::GetSurfaceNormal(Int32 PolyIndex, PolygonObject *op )
//{
//     // Variables
//     CPolygon *pNeighborPoly;
// 
//     Vector v1, v2, vNorm = Vector(0, 1, 0);
//     CPolygon *m_pPolys = op->GetPolygonW();
//     Vector *m_pPoints = op->GetPointW();
//
//
//
//		 pNeighborPoly = &m_pPolys[PolyIndex];
//	     
//		 // Compute face normal
//		 if (pNeighborPoly->c == pNeighborPoly->d)     //triangle
//		 {
//			  v1 = m_pPoints[pNeighborPoly->b] - m_pPoints[pNeighborPoly->a];
//			  v2 = m_pPoints[pNeighborPoly->c] - m_pPoints[pNeighborPoly->a];
//		 }
//		 else     //quadrangle
//		 {
//			  v1 = m_pPoints[pNeighborPoly->c] - m_pPoints[pNeighborPoly->a];
//			  v2 = m_pPoints[pNeighborPoly->d] - m_pPoints[pNeighborPoly->b];
//		 }
//	     
//		 vNorm += v1 % v2; // get cross-product
//
//
//     return !vNorm;
//}




//-------------------------------------------------------------------------------------
//����ʂ̖@���x�N�g����Ԃ�
Vector DTOOL::GetNormlVector(Matrix Mtg)
{
	Matrix mm=Mtg;
	mm.off=Vector(0,0,0);
	//����ʃx�N�g���@�����l��Y����
	Vector vl1=Vector(1,0,0);
	Vector vl2=Vector(0,0,1);
	vl1=mm*vl1;	//�����}�g���b�N�X�Ŋ���ʃx�N�g����ϊ�
	vl2=mm*vl2;	//
	//Vector n=vl2%vl1; //�O�ς��Z�o
	Vector n=Cross(vl2, vl1);

	n.Normalize(); //�x�N�g���𐳋K��
	return n;

}


//----------------------------------------------------------------------------------
Bool DTOOL::ViewSet( BaseDraw *bd,Vector *vm )
{
	Int32 view=bd->GetProjection();

	if(view==4 || view==5) //front &back
	{
		*vm=Vector(0,0,1);
	}
	else if(view==3 || view==2) //right&left
	{
		*vm=Vector(1,0,0);
	}
	else if(view==6 || view==7) //top&bottom
	{
		*vm=Vector(0,1,0);
	}

	return true;
}

//-------------------------------------------------------------------------------------
//�p�x�Œ�
Vector DTOOL::ConstAngleFase( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2,  Float Ang, Matrix mg )
{
	if(!doc) return Vector();


	//	Vector rv=MatrixToHPB(mg,ROTATIONORDER_DEFAULT);
	//printcheckR("ConstAngleFas=",rv);



	mg.off=Vector(0,0,0);
	//v1=!mg*v1;
	//v2=!mg*v2;

	Vector L1=v2-v1;//�}�E�X���W����P�_�ڂ̍��W������
	L1 = ~mg*L1;
	Vector L2=Vector(Len(L1),0,0);



	Vector p1=L1;
	L1.Normalize();
	L2.Normalize();

	//Float rad =VectorAngle(L1,L2);
	Float rad = GetAngle(L1, L2);

	//Vector g=L2%L1; //�O�ς��Z�o
	Vector g = Cross(L2, L1); //�O�ς��Z�o

	if(g.y>0)
	{
		rad *= -1;
	}
	//Float q= atan2 (L2.z,L2.x);
	Float anP=Deg(rad)/Deg(Ang);
	
	anP += 0.5;

	Int32 Ap = (Int32)Floor(anP); //�����_�؂�̂�

	Ang=Ap*Ang;

	Matrix rx=MatrixRotY(Ang);

	Vector LL=Vector(Len(p1),0,0);
	LL=rx*LL;

	Vector f=mg*LL;
	f +=v1;

	return f;
}


//-------------------------------------------------------------------------------
//�p�x�Œ�
Vector DTOOL::ConstAngle( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2, Vector v3, Float Ang, Matrix mg )
{
	if(!doc) return Vector();
	mg.off=Vector(0,0,0);

	//v1=!mg*v1;
	//v2=!mg*v2;
	//v3=!mg*v3;

	Vector L1=v3-v1;//�}�E�X���W����P�_�ڂ̍��W������
	Vector L2=v2-v1;
	L1=~mg*L1;
	L2=~mg*L2;


	Vector p1=L1;
	L1.Normalize();
	L2.Normalize();

	//Float rad =VectorAngle(L1,L2);
	Float rad =GetAngle(L1,L2);

	//Vector g=L2%L1; //�O�ς��Z�o
	Vector g=Cross(L2,L1); //�O�ς��Z�o

	if(g.y>0)
	{
		rad *= -1;
	}
	Float q= atan2 (L2.z,L2.x);
	Float anP=Deg(rad)/Deg(Ang);
	
	anP += 0.5;

	Int32 Ap = (Int32)Floor(anP); //�����_�؂�̂�

	Ang=Ap*Ang;

	Matrix rx=MatrixRotY(Ang+q);

	Vector LL=Vector(Len(p1),0,0);
	LL=rx*LL;

	Vector f=mg*LL;
	f +=v1;

	return f;
}
//-------------------------------------------------------------------------------------
//�O���b�h�X�i�b�v
Bool DTOOL::SetGridPoint( BaseDocument *doc, BaseDraw *bd, BaseContainer &data, Vector G_point, Matrix mg, Float x, Float y, Int32 G_STEP, Bool PL_flag, Matrix PLmg, Vector* G_markpoint)
{
	if(!doc)return false;
	Vector pos=*G_markpoint;
	Vector P=*G_markpoint;
	Bool sflag=false;

	static Bool flagX,flagZ;
	Int32 Val=data.GetInt32(MDATA_GRID_VALUE);

	Vector G1=Vector(0,0,0);
	
	if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc))
	{
		G1=data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET,doc)->GetMg().off;
	}
	
	if(PL_flag)
	{
		G1=PLmg.off;
	}

	if(G_STEP) G1=G_point;

	P= ~mg*P;
	Vector L1= ~mg*G1;
	Float Lx =P.x-L1.x;
	Lx =Lx/Val;
	Float x1=Floor(Lx+0.5);
	Float Lz =P.z-L1.z;
	Lz =Lz/Val;
	Float z1=Floor(Lz+0.5);

	if(data.GetBool(SNAP_GRIDG) && data.GetBool(SNAP_GRIDLINEG))
	{

		Float a =(x1*Val)+L1.x;
		if(Abs(P.x-a) < 20 )
		{
			P.x=a;
			sflag=true;
		}

		Float b = (z1*Val)+L1.z;
		if(Abs(P.z-b) < 20 )
		{
			P.z=b;
			sflag=true;
		}
		P.y=L1.y;
		P=mg*P;
	}
	else
	{
		if(data.GetBool(SNAP_GRIDG))
		{
			P.x=(x1*Val)+L1.x;
			P.z=(z1*Val)+L1.z;
		}

		if(data.GetBool(SNAP_GRIDLINEG))
		{
			Float a =(x1*Val)+L1.x;
			Float b = (z1*Val)+L1.z;

			if(!flagZ)
			{
				if(Abs(P.x-a) < 20 )
				{
					P.x=a;
					flagX=true;
					sflag=true;
				}
				else
				{
					flagX=false;
					sflag=false;
				}
			}
			if(!flagX)
			{
				if(Abs(P.z-b) < 20 )
				{
					P.z=b;
					flagZ=true;
					sflag=true;
				}
				else
				{
					flagZ=false;
					sflag=false;
				}
			}
		}
		P.y=L1.y;
		P=mg*P;
	}
	if(Abs(Len(P-pos)) < 10)
	{
	
		*G_markpoint=P;
		return true;
	}
	if(sflag)
	{
		*G_markpoint=P;
		return true;
	}
	*G_markpoint=pos;
	return false;
}

//-------------------------------------------------------------------------------------
Bool DTOOL::SetGridLine(BaseDocument* doc, BaseContainer& data)
{
	if(!doc) return false;
	BaseDraw* bd=doc->GetActiveBaseDraw();
	GeData gr=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SPACING);
	gr.SetInt32(data.GetInt32(MDATA_GRID_VALUE));
	bd->SetParameter(BASEDRAW_DATA_SNAP_PLANE_SPACING, gr,DESCFLAGS_SET_USERINTERACTION);
//	GePrint("nnn");
	return true;
}


//-------------------------------------------------------------------------------
//��_�X�i�b�v
Bool DTOOL::SP_CrossSnap( BaseDocument *doc, BaseDraw *bd, BaseContainer &data,  Float mx, Float my, Vector *Vspos,  Matrix *Amg )
{
	AutoAlloc<C4DObjectList> objList;
	if(!objList) return false;
	if(!SelectionListCreate(doc, NULL, bd, mx, my, NULL, objList)) return false;
	Int32 Acnt  = objList->GetCount();
	if(!Acnt) return false;

	AutoAlloc <AtomArray> arr;
	if(Acnt)
	{
		for(Int32 i=0; i<Acnt; i++ )
		{
			Int32 Ctype=objList->GetObject(i)->GetType();
			if( (Ctype == Ospline || Ctype== Osplineprofile || Ctype == Osplineflower || Ctype == Osplinetext || Ctype== Osplinenside || Ctype == Ospline4side || Ctype == Osplinecircle || Ctype == Osplinearc || Ctype == Osplinecissoid || Ctype == Osplinecycloid || Ctype == Osplinehelix || Ctype == Osplinerectangle || Ctype == Osplinestar || Ctype == Osplinecogwheel || Ctype == Osplinecontour))
			{
				arr->Append(objList->GetObject(i));
			}
		}
		Acnt=arr->GetCount();
	}
	for(Int32 i=0; i<Acnt; i++ )
	{
		for(Int32 j=i+1; j<Acnt; j++)
		{
			BaseObject *BpA=(BaseObject*)arr->GetIndex(i);
			if(!BpA) return false;
			BaseObject *BpB=(BaseObject*)arr->GetIndex(j);
			if(!BpB) return false;
			Int32 typeA =BpA->GetType();
			Int32 typeB =BpB->GetType();
			Bool closeA, closeB;
			closeA=closeB=false;

			if( (typeA == Ospline || typeA == Osplineprofile || typeA == Osplineflower || typeA == Osplinetext || typeA == Osplinenside || typeA == Ospline4side || typeA == Osplinecircle || typeA == Osplinearc || typeA == Osplinecissoid || typeA == Osplinecycloid || typeA == Osplinehelix || typeA == Osplinerectangle || typeA == Osplinestar || typeA == Osplinecogwheel || typeA == Osplinecontour) && (typeB == Ospline || typeB == Osplineprofile || typeB == Osplineflower || typeB == Osplinetext || typeB == Osplinenside || typeB == Ospline4side || typeB == Osplinecircle || typeB == Osplinearc || typeB == Osplinecissoid || typeB == Osplinecycloid || typeB == Osplinehelix || typeB == Osplinerectangle || typeB == Osplinestar || typeB == Osplinecogwheel || typeB == Osplinecontour) ) //�X�v���C���̏ꍇ
			{
				SplineObject *SpA=NULL;
				SplineObject *SpB=NULL;

				Matrix gmgA,gmgB;

				//	�ҏW�X�v���C���ȊO
				if(typeA != Ospline)
				{
					gmgA=BpA->GetMg();
					SpA=BpA->GetRealSpline();
					

					if(!SpA) return false;
					if(typeA == Osplinearc || typeA == Osplinehelix || typeA == Osplinecycloid || typeA == Osplineformula || typeA == Osplinecissoid )
					{
					}
					else
					{
						closeA=true;
					}

				}
				else//�ҏW�X�v���C���̏ꍇ
				{
					closeA=BpA->GetDataInstance()->GetBool(SPLINEOBJECT_CLOSED);				
					gmgA=BpA->GetMg();

					SpA=ToSpline(BpA);
					if(!SpA) return false;
				}

				//�ҏW�X�v���C���ȊO
				if(typeB != Ospline)
				{
					gmgB=BpB->GetMg();
					SpB=BpB->GetRealSpline();
					if(!SpB) return false;

					if(typeB == Osplinearc || typeB == Osplinehelix || typeB == Osplinecycloid || typeB == Osplineformula || typeB == Osplinecissoid )
					{
					}
					else
					{
						closeB=true;
					}

				}
				else//�ҏW�X�v���C���̏ꍇ
				{
					closeB=BpB->GetDataInstance()->GetBool(SPLINEOBJECT_CLOSED);
					gmgB=BpB->GetMg();

					SpB=ToSpline(BpB);
					if(!SpB) return false;
				}

				//SpA->InitLength(0, NULL);
				//SpB->InitLength(0, NULL);
				Vector *PvA;
				Vector *PvB;
				Int32 PcntLA,PcntLB;

				//////���C���I�u�W�F�N�g�ϊ�
				LineObject *LobjA=SpA->GetLineObject(doc, 1.0f, NULL);
				if(!LobjA) return false;

				PvA=LobjA->GetPointW();//�|�C���g�x�N�g���z��
				PcntLA=LobjA->GetPointCount();	//�|�C���g��
				//�X�v���C�������Ă����ꍇ
				if(closeA)
				{
					PcntLA=PcntLA+=1;
					//PvA = (Vector *) GeAlloc(PcntLA *sizeof(Vector));

					PvA =  NewMemClear(Vector, PcntLA);


					if(!PvA)
					{
						//GeFree(PvA);
						DeleteMem(PvA);

						return false;
					}
					Vector *av=LobjA->GetPointW();//�|�C���g�x�N�g���z��
					if(!av) return false;
					for(Int32 k=0; k<PcntLA-1; k++)
					{
						PvA[k]=av[k];
					}
					PvA[PcntLA-1]=PvA[0];
				}

				////���C���I�u�W�F�N�g�ϊ�
				LineObject *LobjB=SpB->GetLineObject(doc, 1.0f, NULL);
				if(!LobjB) return false;

				PvB=LobjB->GetPointW();//�|�C���g�x�N�g���z��
				PcntLB=LobjB->GetPointCount();	//�|�C���g��
				//�X�v���C�������Ă����ꍇ
				if(closeB)
				{
					PcntLB=PcntLB+=1;
				//	PvB = (Vector *) GeAlloc(PcntLB *sizeof(Vector));

					PvB =  NewMemClear(Vector, PcntLB);

					if(!PvB)
					{
						//GeFree(PvB);
						DeleteMem(PvB);

						return false;
					}
					Vector *av=LobjB->GetPointW();//�|�C���g�x�N�g���z��
					if(!av) return false;
					for(Int32 k=0; k<PcntLB-1; k++)
					{
						PvB[k]=av[k];
					}
					PvB[PcntLB-1]=PvB[0];
				}

				Vector cross=Vector(0,0,0);
				Float t1, t2;
				for(Int32 k=0; k<PcntLA-1; k++)
				{
					for(Int32 p=0; p<PcntLB-1; p++)
					{
						if(!CrossSection( gmgA*PvA[k] , gmgA* PvA[k+1],gmgB*PvB[p], gmgB*PvB[p+1], &cross, data, t1, t2, Amg) && 0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1)
						{

							if(FuzzyPointInRange(data, bd, cross, mx, my))
							{
								*Vspos =cross;
								
								//SpA->FreeLength();
								//SpB->FreeLength();

								blDelete(LobjA);
								blDelete(LobjB);
								//LineObject::Free(LobjA);
								//LineObject::Free(LobjB);
								return true;
							}
						}
					}
				}

				//SpA->FreeLength();
				//SpB->FreeLength();

				blDelete(LobjA);
				blDelete(LobjB);

				//LineObject::Free(LobjA);
				//LineObject::Free(LobjB);
			}
			else
			{
				return false;
			}
		}
	}
	return false;

}





//�N�����ɃL�[�R�[�h�`�F�b�N
Bool DTOOL::KeyCoodCheck(Int32 SERIAL_SIZE )
{
	Int32 Type;
		//Char�^�ɕϊ�
	String ST1;
	//�V���A������key���擾
	GetNumber(ST1,Type );
	//GeOutString(ST1,GEMB_OK);
	Char *Cstr;
	Cstr = NewMemClear(Char, sn_size);
	if(!Cstr)
	{
		DeleteMem(Cstr);
		return false;
	}
	ST1.GetCString(Cstr,sn_size); //��string����Char�^�ɕϊ�

	//PluginID��Char�^�Ŏ擾
	Char data[sn_size];
	if( Type == 1 )
	{
		if (!ReadRegInfo(SERIAL_SIZE,data,sn_size)) return false;
	}
	else
	{
		if (!ReadPluginInfo(SERIAL_SIZE,data,sn_size)) return false;
	}

	//String s=String(data,StXbit);
	//GeOutString(s,GEMB_OK);
	for(Int32 i=0; i<sn_size; i++)
	{
		if(data[i]!=Cstr[i])
		{
		//	GeOutString(String.valueOf(cs),GEMB_OK);
			DeleteMem(Cstr);
			return false;
		}
	}

	DeleteMem(Cstr);
	return true;
}


Bool DTOOL::GetNumber( String &st, Int32 &NUM)
{
 	String c4d_sn;
	SerialInfo si;
	GeGetSerialInfo(SERIALINFO_MULTILICENSE, &si);

	if(si.nr.Content())
	{
		c4d_sn = si.nr;
		NUM=1;
		
	}  else {

		GeGetSerialInfo(SERIALINFO_CINEMA4D,&si);
		c4d_sn = si.nr;
		NUM=2;
	}

	String sn1;
	sn1=c4d_sn.SubStr(0,10); // "fgh"

	Int32 Lsn=sn1.ToInt32();
	st=GetPSN(Lsn);
	return true;
}

//C4d�V���A������v���O�C���V���A������
String DTOOL::GetPSN(const Int32 csn)
{

	//Float a=6748372729; //R13
	// Float a=9387528534; //R14

	//return geneSN( csn, a, "KF", "dkt");

	// Float a=5067745768;
	//return geneSN( csn, a, "TH", "qdj");


	Float a = 22689325865;
	return geneSN(csn, a, "Fw", "Afh");
}

String DTOOL::ExchangeKye(Int32 num)
{
	String s=String::IntToString(num);	//�X�g�����O�^�ɕϊ�

	Int32 cnt=s.GetLength();	//���������擾

	String a, b,c;
	for(Int32 i=0; i<cnt; i++)
	{
		a=s.SubStr(i, 1);
		if(i==0) a="3";
		if(a=="0") b="A";
		if(a=="1") b="B";
		if(a=="2") b="C";
		if(a=="3") b="D";
		if(a=="4") b="E";
		if(a=="5") b="F";
		if(a=="6") b="G";
		if(a=="7") b="H";
		if(a=="8") b="I";
		if(a=="9") b="J";
		c.Insert(i,b);
	}
	return c;
}




///-------------------------------------------------------------------------------------------
//���s���[�h���ʒu��Ԃ�
Vector DTOOL::GetDrawVec( BaseDocument *doc, BaseContainer &data, Vector v1, Vector v2, Matrix mg )
{
	//Matrix amg;
	//���[�J����������ɍ������߂�
	//�x�N�g�������[�J����
	v1=~mg*v1;
	v2=~mg*v2;
	Vector v3=v2-v1;

	Float x =Abs(v3.x);
	Float z =Abs(v3.z);
	if(x < z)
	{
		v2.x=v1.x;
	}
	else if(x > z)
	{
		v2.z=v1.z;
	}

	v2=mg*v2;

	return v2;
}


String DTOOL::geneSN( Int32 sn, Float ch, String s1, String s2)
{

	String st=String::IntToString(sn);

	String a1=st.SubStr(0,5);

	String a2=st.SubStr(5,12);


	Float t1=a1.ToFloat();
	Float t2=a2.ToFloat();


	Float t3=t1*t2;

	t3=t3*t1;
	t3+=ch;

	String pw=String::FloatToString(t3);

	pw=pw.SubStr(0,10);
	
	pw=s1+pw.SubStr(5,11)+s2+pw.SubStr(0,5);
	return pw;

}

//======================================================================
//�Q�̃x�N�g���Ԃ̊p�x��Ԃ�
Float DTOOL::VectorToRad(Vector v1, Vector v2, Matrix mg)
{
	v1=~mg*v1;
	v2=~mg*v2;
	v1.Normalize();
	v2.Normalize();
	//Float rad =VectorAngle(v1,v2);
	Float rad =GetAngle(v1,v2);

	//Vector g=v1%v2; //�O�ς��Z�o
	Vector g=Cross(v1,v2); //�O�ς��Z�o

	if(g.z<0)
	{
		rad *= -1;
	}

//	if(g.z<0)
//{
//	rad = Rad(360.0)-rad;
//}
	return rad;	
}




//***********************************************************************************
//�V�X�e���̃X�i�b�v�ݒ���c�[���̃X�i�b�v�ݒ�ɔ��f
//***********************************************************************************
////
Bool DTOOL::CheckSnapSetting( BaseDocument* doc, BaseContainer& data)
{
	
	if(!doc ) return false;
	BaseContainer bc = SnapSettings(doc);

	data.SetBool(SNAP_ENABLEG, bc.GetBool(SNAP_SETTINGS_ENABLED));
	if(bc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_AUTO)
	{
	//	GePrint("SNAP_SETTINGS_MODE_AUTO");
		data.SetInt32(SNAP_METHODG, SNAP_AUTO);
	}
	else if(bc.GetInt32(SNAPMODE_COMBO) == SNAP_SETTINGS_MODE_2D)
	{
	//	GePrint("SNAP_SETTINGS_MODE_2D");
		data.SetInt32(SNAP_METHODG, SNAP_2DG);
	}
	else
	{
		//GePrint("SNAP_METHODG, SNAP_3");
		data.SetInt32(SNAP_METHODG, SNAP_3DG);
	}
	data.SetInt32(SNAP_RADIUSG, (Int32)bc.GetFloat(SNAP_SETTINGS_RADIUS));
	data.SetBool(SNAP_QUANTIZUNG, bc.GetBool(QUANTIZE_ENABLED));

	data.SetBool(SNAP_POINTG, IsSnapEnabled(doc,SNAPMODE_POINT) );
	data.SetBool(SNAP_SPLINEG, IsSnapEnabled(doc,SNAPMODE_SPLINE ) );
	data.SetBool(SNAP_MIDPOINTG, IsSnapEnabled(doc,SNAPMODE_MIDPOINT ) );
	data.SetBool(SNAP_EDGEG, IsSnapEnabled(doc,SNAPMODE_EDGE ) );
	data.SetBool( SNAP_AXISG, IsSnapEnabled(doc, SNAPMODE_AXIS ) );
	data.SetBool( SNAP_POLYGONG, IsSnapEnabled(doc, SNAPMODE_POLYGON ) );
	data.SetBool( SNAP_INTERSECTIONG, IsSnapEnabled(doc, SNAPMODE_INTERSECTION ) );
	data.SetBool( SNAP_WORKPLANEG, IsSnapEnabled(doc, SNAPMODE_WORKPLANE ) );
	data.SetBool( SNAP_GRIDG, IsSnapEnabled(doc, SNAPMODE_GRIDPOINT ) );
	data.SetBool( SNAP_GRIDLINEG, IsSnapEnabled(doc, SNAPMODE_GRIDLINE) );
	data.SetBool( SNAP_GUIDE_SNAPG, IsSnapEnabled(doc, SNAPMODE_GUIDE) );
	data.SetBool( SNAP_DYNAMICGUIDE_SNAPG, IsSnapEnabled(doc, SNAPMODE_DYNAMICGUIDE) );
	data.SetBool( SNAP_PERPENDICULARG, IsSnapEnabled(doc, SNAPMODE_ORTHO) );


	//�O���b�h�ݒ�
	BaseDraw* bd = doc->GetActiveBaseDraw();
	GeData vd=bd->GetParameterData(BASEDRAW_DATA_PLANE_LEGACY_MODE);
	data.SetBool(MDATA_BOOL_GRID_FLAG, vd.GetBool());

	vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SPACING);
	data.SetFloat(MDATA_GRID_VALUE,vd.GetInt32() );

	vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SUB);
	data.SetInt32(MDATA_GRIDLINE,vd.GetInt32() );
	vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_ROUGHSUB);
	data.SetInt32(MDATA_LINE,vd.GetInt32() );

	vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_DYNAMICGRID);
	switch(vd.GetInt32())
	{
	case 0:
		data.SetInt32(MDATA_GRID_LIST,CH_GRID_L0 );
		break;
	case 1:
		data.SetInt32(MDATA_GRID_LIST,CH_GRID_L1 );
		break;
	case 2:
		data.SetInt32(MDATA_GRID_LIST,CH_GRID_L2 );
		break;
	case 3:
		data.SetInt32(MDATA_GRID_LIST,CH_GRID_L3 );
		break;
	case 4:
		data.SetInt32(MDATA_GRID_LIST,CH_GRID_L4 );
		break;
	}
	return true;
}

  //*************************************************************
//�c�[���̃X�i�b�v�ݒ肩��V�X�e���̃X�i�b�v�ݒ�����킹��
//*************************************************************
Bool DTOOL::SnapSync(BaseDocument* doc, BaseContainer& data, Bool PlaneSet_BTNFlag)
{

	SetCHECKUP(doc, data, PlaneSet_BTNFlag);

	if (!doc) return false;
	EnableSnap(data.GetBool(SNAP_POINTG), doc, SNAPMODE_POINT);
	EnableSnap(data.GetBool(SNAP_SPLINEG), doc, SNAPMODE_SPLINE);
	EnableSnap(data.GetBool(SNAP_MIDPOINTG), doc, SNAPMODE_MIDPOINT);
	EnableSnap(data.GetBool(SNAP_EDGEG), doc, SNAPMODE_EDGE);
	EnableSnap(data.GetBool(SNAP_AXISG), doc, SNAPMODE_AXIS);
	EnableSnap(data.GetBool(SNAP_POLYGONG), doc, SNAPMODE_POLYGON);
	EnableSnap(data.GetBool(SNAP_INTERSECTIONG), doc, SNAPMODE_INTERSECTION);
	EnableSnap(data.GetBool(SNAP_WORKPLANEG), doc, SNAPMODE_WORKPLANE);
	EnableSnap(data.GetBool(SNAP_GRIDG), doc, SNAPMODE_GRIDPOINT);
	EnableSnap(data.GetBool(SNAP_GRIDLINEG), doc, SNAPMODE_GRIDLINE);
	EnableSnap(data.GetBool(SNAP_GUIDE_SNAPG), doc, SNAPMODE_GUIDE);
	EnableSnap(data.GetBool(SNAP_DYNAMICGUIDE_SNAPG), doc, SNAPMODE_DYNAMICGUIDE);
	EnableSnap(data.GetBool(SNAP_PERPENDICULARG), doc, SNAPMODE_ORTHO);


	EventAdd();
	return true;
}
//***********************************************************************************
//�O���b�h�A�X�i�b�v�A���ʁ@�ݒ���V�X�e���ɔ��f������
//***********************************************************************************
void DTOOL::SetCHECKUP(BaseDocument* doc, BaseContainer& data, Bool flag)
{



	Float cnt=data.GetFloat(MDATA_GRID_VALUE);
	data.SetFloat(MDATA_GRID_VALUE, floor(cnt));

	BaseDraw* bd = doc->GetActiveBaseDraw();
	GeData vd=bd->GetParameterData(BASEDRAW_DATA_PLANE_LEGACY_MODE);
	vd.SetInt32(data.GetBool(MDATA_BOOL_GRID_FLAG));
	bd->SetParameter(BASEDRAW_DATA_PLANE_LEGACY_MODE,vd,DESCFLAGS_SET_USERINTERACTION);

	if(data.GetBool(MDATA_BOOL_GRID_FLAG))
	{
		vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SPACING);
		vd.SetFloat(data.GetFloat(MDATA_GRID_VALUE));
		bd->SetParameter(BASEDRAW_DATA_SNAP_PLANE_SPACING,vd,DESCFLAGS_SET_USERINTERACTION);

		vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SUB);
		vd.SetInt32((Int32)data.GetFloat(MDATA_GRIDLINE));
		bd->SetParameter(BASEDRAW_DATA_SNAP_PLANE_SUB,vd,DESCFLAGS_SET_USERINTERACTION);


		vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_ROUGHSUB);
		vd.SetInt32((Int32)data.GetFloat(MDATA_LINE));
		bd->SetParameter(BASEDRAW_DATA_SNAP_PLANE_ROUGHSUB,vd,DESCFLAGS_SET_USERINTERACTION);

		vd=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_DYNAMICGRID);
		switch( data.GetInt32(MDATA_GRID_LIST))
		{
		case CH_GRID_L0:
			vd.SetInt32(0);
			break;
		case CH_GRID_L1:
			vd.SetInt32(1);
			break;
		case CH_GRID_L2:
			vd.SetInt32(2);
			break;
		case CH_GRID_L3:
			vd.SetInt32(3);
			break;
		case CH_GRID_L4:
			vd.SetInt32(4);
			break;
		}
		bd->SetParameter(BASEDRAW_DATA_SNAP_PLANE_DYNAMICGRID,vd,DESCFLAGS_SET_USERINTERACTION);
	}


	//*******************************************************
	//�X�i�b�v
	//*******************************************************
	BaseContainer bc = SnapSettings(doc);
	EnableSnap(data.GetBool(SNAP_ENABLEG), doc);

	bc.SetBool(SNAP_SETTINGS_ENABLED, data.GetBool( SNAP_ENABLEG));
	if(data.GetInt32(SNAP_METHODG) == SNAP_AUTO)
	{
		bc.SetInt32(SNAP_SETTINGS_MODE,  SNAP_SETTINGS_MODE_AUTO);
	}
	else if(data.GetInt32(SNAP_METHODG) ==  SNAP_2DG)
	{
		bc.SetInt32(SNAP_SETTINGS_MODE,  SNAP_SETTINGS_MODE_2D);
	}
	else
	{
		bc.SetInt32(SNAP_SETTINGS_MODE,  SNAP_SETTINGS_MODE_3D);
	}

	bc.SetBool(QUANTIZE_ENABLED, data.GetBool( SNAP_QUANTIZUNG));
	bc.SetFloat(SNAP_SETTINGS_RADIUS, (Float)data.GetInt32(SNAP_RADIUSG));

	SnapSettings(doc, bc);
    
}


////***********************************************************************************
////����ʐݒ�
////***********************************************************************************
//void DTOOL::SetObjectPlane(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool Pflag, Matrix* Amg)
//{
//
//	
//	Int32 view =	bd->GetProjection();
//	//����ʃI�u�W�F�N�g���擾
//	BaseObject* plane = GetWorkplaneObject(doc);
//
//	//�ݒ肵�Ă��镽�ʎ��^�C�v
//	Int32 Ptype=data.GetInt32(IDE_Snap_AXIS);
//
//
//	//�C�ӂ̕��ʂ�ݒ肵�Ă���ꍇ
//	if ( Pflag || data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
//	{
//		//�r���[���I���\���[�h & ����World�I�v�V�������L���̏ꍇ
//		if ((view > 1 && view < 8) && data.GetBool(MADATA_SW_PLANEWORLD)) //�r���[���I���\���[�h�̏ꍇ
//		{
//			//���ʂ����Z�b�g
//			Matrix mg;
//			plane->SetMg(mg);
//
//		}
//		else //�I���\�[�r���[�ȊO
//		{
//			Matrix Bmg;
//			if (Pflag) //�N���b�N���ʂ��L���̏ꍇ
//			{
//				Bmg = *Amg;
//			}
//			else if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)) //�^�[�Q�b�g�X���b�g���L���̏ꍇ
//			{				
//				//�^�[�Q�b�g�I�u�W�F�N�g�̃}�g���b�N�X���擾
//				Bmg = data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)->GetMg();
//			}
//
//			//�ݒ�ɍ��킹�Ċ���ʂ���]
//			Matrix r;
//			if (Ptype == IDE_XY)
//			{
//				r = MatrixRotX(Rad(90.0));
//			}
//			else if (Ptype == IDE_YZ)
//			{
//				r = MatrixRotZ(Rad(-90.0));
//			}
//
//			Vector pos = Bmg.off;
//			Bmg.off = Vector();
//			Bmg = Bmg*r;
//			Bmg.off = pos;
//
//
//			//�����̃N���b�N�}�g���b�N�X������ʃI�u�W�F�N�g�ɐݒ�
//			plane->SetMg(Bmg);
//			*Amg = Bmg;
//		}
//
//	}
//	else //�����ݒ肵�Ă��Ȃ��ꍇ
//	{
//		//�r���[���I���\���[�h & ����World�I�v�V�������L���̏ꍇ
//		if ((view > 1 && view < 8) && data.GetBool(MADATA_SW_PLANEWORLD)) 
//		{
//
//			//����ʂ̓��Z�b�g���
//			Matrix resetmg;
//			plane->SetMg(resetmg);
//		}
//		else   //�I���\�[�r���[�ȊO
//		{
//			Matrix r;
//			if (Ptype == IDE_XY)
//			{
//				r = MatrixRotX(Rad(90.0));
//			}
//			else if (Ptype == IDE_YZ)
//			{
//				r = MatrixRotZ(Rad(-90.0));
//			}
//			plane->SetMg(r);
//
//		}
//	}
//





//	BaseDraw* bd=doc->GetActiveBaseDraw();
//	WorkplaneLock( bd, true);
	//BaseObject* plane= GetWorkplaneObject( doc);

	//Matrix mg;
	//Vector pos;

	////�^�[�Q�b�g���ݒ肳��Ă���ꍇ
	//if(data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET ,doc))
	//{
	//	mg=data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET ,doc)->GetMg();
	//}
	//
	////�C�ӂ̕��ʂ��ݒ肳��Ă���ꍇ
	//if(flag)
	//{
	//	mg=plane->GetMg();
	//}

	//Int32 view =	bd->GetProjection();
	//pos=mg.off;
	//Matrix r;
	//if(view==4 || view==5 || view==3 || view==2)
	//{

	//}
	//else
	//{
	//	if( data.GetInt32(IDE_Snap_AXIS)==IDE_XY || data.GetInt32(IDE_Snap_AXIS)==IDE_YZ)
	//	{
	//	
	//		if(data.GetInt32(IDE_Snap_AXIS)==IDE_XY)
	//		{
	//			r=MatrixRotX(Rad(90.0));
	//		}
	//		else
	//		{
	//			r=MatrixRotZ(Rad(-90.0));
	//		}
	//		mg=mg*r;
	//	}
	//}

	//mg.off=pos;
	//plane->SetMg(mg);
//}

// //***********************************************************************************
////����ʐݒ�
////�I���\�r���[�̏ꍇ�͍�ƕ��ʂ̓��Z�b�g
//// Amg�F���[�U�[�}�g���b�N�X�A�p�[�X�y�N�e�B�u�̏ꍇ�͐ݒ蕽�ʂ̃}�g���b�N�X����
////***********************************************************************************
//void DTOOL::SetObjectPlane2(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool Pflag, Matrix Amg)
//{
//
//	Int32 view = bd->GetProjection();
//	//����ʃI�u�W�F�N�g���擾
//	BaseObject* plane = GetWorkplaneObject(doc);
//
//	//�ݒ肵�Ă��镽�ʎ��^�C�v
//	Int32 Ptype = data.GetInt32(IDE_Snap_AXIS);
//
//
//	//�C�ӂ̕��ʂ�ݒ肵�Ă���ꍇ
//	if (Pflag || data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
//	{
//		//�r���[���I���\���[�h & ����World�I�v�V�������L���̏ꍇ
//		if ((view > 1 && view < 8) && data.GetBool(MADATA_SW_PLANEWORLD)) //�r���[���I���\���[�h�̏ꍇ
//		{
//			//���ʂ����Z�b�g
//			Matrix mg;
//			plane->SetMg(mg);
//
//		}
//		else //�I���\�[�r���[�ȊO
//		{
//			Matrix Bmg;
//			if (Pflag) //�N���b�N���ʂ��L���̏ꍇ
//			{
//				Bmg = Amg;
//			}
//			else if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)) //�^�[�Q�b�g�X���b�g���L���̏ꍇ
//			{
//				//�^�[�Q�b�g�I�u�W�F�N�g�̃}�g���b�N�X���擾
//				Bmg = data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)->GetMg();
//			}
//
//			//�ݒ�ɍ��킹�Ċ���ʂ���]
//			Matrix r;
//			if (Ptype == IDE_XY)
//			{
//				r = MatrixRotX(Rad(90.0));
//			}
//			else if (Ptype == IDE_YZ)
//			{
//				r = MatrixRotZ(Rad(-90.0));
//			}
//
//			Vector pos = Bmg.off;
//			Bmg.off = Vector();
//			Bmg = Bmg*r;
//			Bmg.off = pos;
//
//
//			//�����̃N���b�N�}�g���b�N�X������ʃI�u�W�F�N�g�ɐݒ�
//			plane->SetMg(Bmg);
//			//*Amg = Bmg;
//		}
//
//	}
//	else //�����ݒ肵�Ă��Ȃ��ꍇ
//	{
//		//�r���[���I���\���[�h & ����World�I�v�V�������L���̏ꍇ
//		if ((view > 1 && view < 8) && data.GetBool(MADATA_SW_PLANEWORLD))
//		{
//
//			//����ʂ̓��Z�b�g���
//			Matrix resetmg;
//			plane->SetMg(resetmg);
//		}
//		else   //�I���\�[�r���[�ȊO
//		{
//
//			Matrix r;
//			Matrix m;
//			if( Pflag) m= Amg;
//			if (Ptype == IDE_XY)
//			{
//				r = MatrixRotX(Rad(-90.0));
//			}
//			else if (Ptype == IDE_YZ)
//			{
//				r = MatrixRotZ(Rad(90.0));
//			}
//
//			m= plane->GetMg();
//			m=m*r;
//			plane->SetMg(m);
//		}
//	}
//	SetGridLine(doc, data);
//}
//



//***********************************************************************************
//������
//***********************************************************************************
void DTOOL::init(BaseDocument* doc, BaseContainer& data)
{
	//p���t�@�����X�p�X���擾
	Filename Fpath = GeGetC4DPath(C4D_PATH_PREFS);
	String path = Fpath.GetString();
	//���̃f�B���N�g�����擾
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

	//	Int32 length = path.GetLength();
	String lastWord = path.SubStr(0, pos);  //Grab the last word (sub string)

	//plugins�t�H���_�p�X��ݒ�
#ifdef __PC
	{
		lastWord += "\\plugins";
	}
#else
	{
		lastWord += "/plugins";
	}
#endif
	Fpath.SetString(lastWord);

	//Plugins�t�H���_�̊m�F
	if (!GeFExist(Fpath, true)) //Plugin�t�H���_���Ȃ��ꍇ
	{
		if (GeFCreateDir(Fpath)) //Plugin�t�H���_�̍쐬
		{
#ifdef __PC
	{
		lastWord += "\\SnapPRS"; //DragPrimitives�t�H���_�p�X�̐ݒ�
	}
#else
	{
		lastWord += "/SnapPRS"; //DragPrimitives�t�H���_�p�X�̐ݒ�
	}
#endif
			Fpath.SetString(lastWord);
			if (GeFCreateDir(Fpath)) //DragPrimitives�t�H���_�̐���
			{

			}
		}
	}
	else //Plugin�t�H���_������ꍇ
	{
#ifdef __PC
	{
		lastWord += "\\SnapPRS"; //DragPrimitives�t�H���_�p�X�̐ݒ�
	}
#else
	{
		lastWord += "/SnapPRS"; //DragPrimitives�t�H���_�p�X�̐ݒ�
	}
#endif
		Fpath.SetString(lastWord);
		//DragPrimitives�t�H���_�̊m�F
		if (!GeFExist(Fpath, true)) //DragPrimitives�t�H���_���Ȃ��ꍇ
		{
			GeFCreateDir(Fpath); //Plugin�t�H���_�̍쐬

		}
	}

	CheckSnapSetting(doc, data);
}

Vector DTOOL::GetScaleForMatrix(Matrix mg)
{
	Vector scl=Vector(1,1,1);

	scl.x=Len(mg.v1);
	scl.y=Len(mg.v2);
	scl.z=Len(mg.v3);

	return scl;

}

//*******************************************************************
//�Q�̃x�N�g������p�x��Ԃ�
//*******************************************************************
Float DTOOL::Vector2AngleLocal(Vector vv1, Vector vv2, Matrix mg, Int32 &flag)
{
	Vector pos1=vv1;
	pos1=~mg*pos1;
	Vector pos2=vv2;
	pos2=~mg*pos2;

	Vector v1= pos1;
	Vector v0= pos2;
	v1.Normalize();
	v0.Normalize();

	//Float r = VectorAngle(v0,v1);
	 Float r = GetAngle(v0,v1);
	//Vector g=v1%v0; //�O�ς��Z�o
	 Vector g=Cross(v1,v0 ); //�O�ς��Z�o
	if(g.y>0)
	{
		r *= -1;
	}
	if(Deg(r) >0  && Deg(r) < 90)
	{
		flag=0;
	}
	else if(Deg(r) >90  && Deg(r) <180)
	{
		flag=1;
	}
	else if(Deg(r) < 0  && Deg(r) > -90)
	{
		flag=3;
	}
	else if(Deg(r) < -90  && Deg(r) > -180)
	{
		flag=2;
	}
	return r;
}

//********************************************************************************************
//�^�[�Q�b�g���܂Ƃ߂ăI�u�W�F�N�g��Ԃ�
//********************************************************************************************
BaseObject* DTOOL ::GetTargetObj(BaseDocument* doc, BaseObject* TGobj)
{
	BaseObject* TgresA = nullptr;


	Bool deformed = (TGobj->GetDeformCache() != NULL);
	if (!deformed)
	{
		Int32 info;
		for (BaseObject* child = TGobj->GetDown(); child; child = child->GetNext())
		{
			info = child->GetInfo();
			if (child->GetDeformMode() && ((info == OBJECT_MODIFIER) || (info == OBJECT_HIERARCHYMODIFIER)))
			{
				deformed = true;
				GePrint("yes");
				break;
			}
		}
	}
	TgresA = TGobj;

	//Matrix TGmg=TGobj->GetMg();

	//BaseContainer bcc;
	//bcc.SetBool(MDATA_CURRENTSTATETOOBJECT_KEEPANIMATION, true);
	//bcc.SetBool(MDATA_CURRENTSTATETOOBJECT_INHERITANCE, true);
	//bcc.SetBool(MDATA_CURRENTSTATETOOBJECT_NOGENERATE, false);

	//ModelingCommandData mcdA;
	//mcdA.op = TGobj;
	////mcd.arr=arr;
	//mcdA.mode = MODELINGCOMMANDMODE_ALL;
	//mcdA.doc = doc;
	//mcdA.bc = &bcc;
	//SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, mcdA);
	//BaseObject *TGres = static_cast<BaseObject*>(mcdA.result->GetIndex(0));
	//if (!TGres) return nullptr;

	//
	//search_hierarchyDEL( TGres->GetDown(), TGres);


	////�Ώۃ��f�����擾
	//AutoAlloc <AtomArray> arrA;
	//arrA->Append(TGres);
	//search_hierarchy( TGres->GetDown(), TGres, arrA);

	//Matrix Amg=TGobj->GetMl();

	//BaseObject* parent=	TGobj->GetUp();
	////Bool Cflag=false;
	//if(arrA->GetCount() > 1 )
	//{


	//	BaseContainer bc;
	//	bc.SetBool(MDATA_JOIN_MERGE_SELTAGS, true);
	//	//��̉�
	//	ModelingCommandData mcdB;
	//	mcdB.doc =doc;
	//	mcdB.arr=arrA;
	//	mcdB.bc = &bc;
	//	mcdB.mode = MODELINGCOMMANDMODE_ALL;
	//	if (!SendModelingCommand(MCOMMAND_JOIN, mcdB)) return nullptr;
	//	TgresA = static_cast<BaseObject*>(mcdB.result->GetIndex(0));
	//	BaseObject::Free(TGres);

	//}
	//else
	//{

	//	TgresA=TGres;

	//	PointObject* pp=ToPoint(TgresA);
	//	if(!pp) return nullptr;
	//	Vector* vv=pp->GetPointW();

	//	for(Int32 i=0; i<pp->GetPointCount(); i++)
	//	{
	//		vv[i]=Amg*vv[i];
	//	}
	//}

	//if( parent)
	//{

	//	Matrix mp=parent->GetMg();
	//	TgresA->SetMg(mp);
	//}
	//else
	//{
	//	Matrix xmg;
	//	TgresA->SetMl(xmg);
	//}


	TgresA->Message(MSG_UPDATE);

	return TgresA;
}


//�͈̓`�F�b�N ��A����vB�̊ԂȂ�^
Vector DTOOL::LockAngle( BaseDocument *doc, BaseContainer &data, Vector v1, Float Ang, Vector GP1,Vector GP2, Matrix Amg )
{

	//Vector rv=MatrixToHPB(Amg,ROTATIONORDER_DEFAULT);
	//printcheckR("rv=",rv);

	Vector R1=GP1;
	Vector R2=GP2;//���S�߲��

	R1=~Amg*R1; //�P�_��
	R2=~Amg*R2; //2�_��
	Vector v1A=~Amg*v1; //�}�E�X�ʒu

	Vector RL1 = v1A-R1;
	Vector RL2 = R2-R1;

	RL1.Normalize();
	RL2.Normalize();
	//Float rad =VectorAngle(RL1,RL2);
	Float rad =GetAngle(RL1,RL2);


	//Vector g=RL2%RL1; //�O�ς��Z�o

	Vector g=Cross(RL2,RL1); //�O�ς��Z�o

	if(g.y>0) rad *= -1;

	Float Arad=Deg(rad);
	Float anP=Arad/Deg(Ang);
	anP += 0.5;
	//�W�������߂�
	Int32 Ap = (Int32)Floor(anP); //�����_�؂�̂�
	rad=Ap*Deg(Ang);

	//�p�x���Z�o
	rad=Rad(rad);


	Vector Lk;

	Lk=Vector(Len(RL2), 0,0);
		
	//Float Orad =VectorAngle(RL2,Lk);
	Float Orad =GetAngle( RL2,Lk);

	
	//Vector g2=Lk%RL2; //�O�ς��Z�o
	Vector g2=Cross(Lk,RL2); //�O�ς��Z�o

	if(g2.y>0)
	{
		Orad *= -1;
	}
	/*
	*r = rad;
	*r+=Rad(180.0);
	*/
	rad=Orad+rad;
	

	Matrix mg=MatrixRotY(rad);
	Vector gL=v1A-R1;
	Vector LW=Vector(Len(gL),0,0);
	LW=mg*LW;
	Vector v=Amg*LW;
	//v1=R1+RL1;

	v=v+GP1;

	return v;
}


//*********************************************************************
//�}�[�J�[�\���ݒ�
//*********************************************************************
Bool DTOOL::Snap90Plane( BaseDocument *doc, BaseContainer &data, BaseDraw *bd,BaseDrawHelp *bh, Vector G_markpoint,  Matrix SNmg  )
{
	if(!doc) return false;


	//�|�C���^��̃��b�h�|�C���g�\��
	bd->SetPen(Vector(1.0,0.0,0.0));
	bd->SetMatrix_Matrix(NULL, bh->GetMg());
	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
	bd->DrawHandle(G_markpoint,DRAWHANDLE_BIG, NOCLIP_D); 

	//���[�J���}�g���b�N�X�Ɍ��݂̈ʒu�������X�P�[�������߂�
	Matrix point_mg;
	point_mg.off=G_markpoint;

	//GePrint("point_mg.x="+FloatToString(point_mg.off.x)+"point_mg.y="+FloatToString(point_mg.off.y)+"point_mg.z="+FloatToString(point_mg.off.z));
	Int32 left, top, right, bottom, width, height;
	Vector ctr = bd->WS(G_markpoint);
	
	bd->GetFrame(&left, &top, &right, &bottom);
	width = right - left + 1;
	height = bottom - top + 1;
	Float rad = (Float)((width > height)?(height>>2):(width>>2));
	
      // Scale for PolygonObject
	Int32 proj =	bd->GetProjection();
	Vector a = ScreenToWorld(ctr, bd, proj);
	Vector b = ScreenToWorld(Vector(ctr.x+rad*0.1, ctr.y, ctr.z), bd, proj);
	Int32 rd=data.GetInt32(SNAP_RADIUSG);

	Float rad2 = rd * Len(b-a);
	rad2*=0.1;
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

	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
	

      
	Matrix Fmg=point_mg*SNmg;
	Fmg.off=G_markpoint;
	Vector rec[4];
	Float size=1.5;
	rec[0]=Vector(0,0,0);
	rec[1]=Vector(size,0,0);
	rec[2]=Vector(size,0,size);
	rec[3]=Vector(0,0,size);
	bd->SetPen(Vector(255,255,255));
	for(Int32 i=0; i<4; i++)
	{
	Vector v1 =Fmg*rec[i];
	Vector v2;
	if(i==3)
	{
		v2=Fmg*rec[0];
	}
	else
	{
		v2=Fmg*rec[i+1];
	}

		bd->SetMatrix_Matrix(NULL, bh->GetMg());
		bd->DrawLine(v1,v2, NOCLIP_D);
	}
	DrawTextA(" Perpendicular ", G_markpoint, -30,bd, 999 );
	


	return true;
}





//--------------------------------------------------------------
//���������ɕ��ёւ�
//--------------------------------------------------------------
void DTOOL::QSortK(  Mobj *x, Int32 left, Int32 right)
{

	Int32 i, j;
	Int32 pivot;

    i = left;                      /* �\�[�g����z��̈�ԏ������v�f�̓Y�� */
    j = right;                     /* �\�[�g����z��̈�ԑ傫���v�f�̓Y�� */

//	pivot = x[(left + right) / 2].UV_p; /* ��l��z��̒����t�߂ɂƂ� */
	pivot = x->pnum[(left + right) / 2]; /* ��l��z��̒����t�߂ɂƂ� */


	while (1) {                    /* �������[�v */

	//	while (x[i].UV_p.x < pivot.x)       /* pivot ���傫���l�� */
		while (x->pnum[i] < pivot)       /* pivot ���傫���l�� */
			i++;                   /* �o��܂� i �𑝉������� */

		while (pivot < x->pnum[j])       /* pivot ��菬�����l�� */
			j--;                   /*  �o��܂� j ������������ */
		if (i >= j)                /* i >= j �Ȃ� */
			break;                 /* �������[�v���甲���� */

		SwapK( x, i, j);             /* x[i] �� x[j]������ */
		i++;                       /* ���̃f�[�^ */
		j--;
	}
//	ShowData();               /* �r���o�߂�\�� */

	if (left < i - 1)              /* ��l�̍��� 2 �ȏ�v�f������� */
		QSortK( x, left, i - 1);     /* ���̔z��� Q �\�[�g���� */
	if (j + 1 <  right)            /* ��l�̉E�� 2 �ȏ�v�f������� */
		QSortK(x, j + 1, right);    /* �E�̔z��� Q �\�[�g���� */


}


//--------------------------------------------------------------
  /* �z��̗v�f���������� */
//--------------------------------------------------------------
void DTOOL::SwapK( Mobj *x, Int32 i, Int32 j)
{

	Int32 tempL;

	tempL = x->pnum[i];

	x->pnum[i] = x->pnum[j];

	x->pnum[j] = tempL;

}


//-------------------------------------------------------------------------------
//l��_�����߂�
Bool DTOOL::CrossSection(Vector As, Vector Ae, Vector Bs, Vector Be, Vector *cross, BaseContainer &data, Float &t1, Float &t2, Matrix *Amg)
{

	if (Ae == As || Be == Bs) return false;

	Matrix mg = *Amg;
	mg.off = Vector(0, 0, 0);
	As = ~mg*As;
	Ae = ~mg*Ae;
	Bs = ~mg*Bs;
	Be = ~mg*Be;
	Float x1, y1, f1, g1, x2, y2, f2, g2;

	x1 = As.x;
	y1 = As.z;
	f1 = Ae.x - As.x;
	g1 = Ae.z - As.z;
	x2 = Bs.x;
	y2 = Bs.z;
	f2 = Be.x - Bs.x;
	g2 = Be.z - Bs.z;

	// det�̌v�Z
	Float det = f2*g1 - f1*g2;
	if (det == 0)
		return true;             // ������==�ł����܂�Ȃ��Ǝv��

	// ��_�ɂ�����p�����[�^
	Float dx = x2 - x1;
	Float dy = y2 - y1;
	t1 = (f2*dy - g2*dx) / det;
	t2 = (f1*dy - g1*dx) / det;

	// ��_�̍��W
	Float x = x1 + f1*t1;
	Float y = y1 + g1*t1;


	*cross = mg* Vector(x, (As.y + Ae.y) / 2, y);


	return false;
}


//--------------------------------------------------------------
/* �S�̍��W�����_�����߂� */
//--------------------------------------------------------------
Bool DTOOL::CrossSectionB( Vector As, Vector Ae, Vector Bs, Vector Be, Vector *crossA, Vector *crossB, BaseContainer &data, Matrix Amg )
{
	if(Ae == As || Be == Bs) return false;
	Matrix mg=Amg;
	mg.off=Vector();
	As=~mg*As;
	Ae=~mg*Ae;
	Bs=~mg*Bs;
	Be=~mg*Be;
    
    
	Float Aa, Ab, Ba, Bb,cx, cz;
	Float A1,A2, AA1, AA2, B1, B2, BB1, BB2;
    
	Aa=Ab=Ba=Bb=cx=cz=0;
	A1=A2=AA1=AA2=B1=B2=BB1=BB2=0;
    
//	switch (data.GetLong(IDC_EC_PLANE))
//	{
//        case IDE_XZ:
//		{
			A1=As.x;
			A2=Ae.x;
			AA1 = As.z;
			AA2 = Ae.z;
			B1 = Bs.x;
			B2 = Be.x;
			BB1 = Bs.z;
			BB2 = Be.z;
//		}
//            break;
//        case IDE_XY:
//		{
//			A1=As.x;
//			A2=Ae.x;
//			AA1 = As.y;
//			AA2 = Ae.y;
//			B1 = Bs.x;
//			B2 = Be.x;
//			BB1 = Bs.y;
//			BB2 = Be.y;
//		}
//            break;
//        case IDE_YZ:
//		{
//			A1=As.z;
//			A2=Ae.z;
//			AA1 = As.y;
//			AA2 = Ae.y;
//			B1 = Bs.z;
//			B2 = Be.z;
//			BB1 = Bs.y;
//			BB2 = Be.y;
//		}
//            break;
//	}
	//AB�������Ȃ�U
	if(A1==A2 && B1 == B2 ) return false;
    
	//a,b�̌v�Z
	if( A2 != A1)//����A�������łȂ��ꍇ
	{
		Aa = ( AA2 - AA1) / (A2 - A1);
		Ab = AA1 - (Aa *A1);
        
	}
	if( B2 != B1)//����B�������łȂ��ꍇ
	{
		Ba = (BB2 - BB1) / ( B2 - B1);
        
		Bb = BB1 - (Ba * B1);
        
	}
    
	//�Q��A�����Ⴄ�ꍇ
	if(A1 != A2 )
	{
		if(B1 != B2 )
		{
			if(Aa == Ba) return false;//����A�ƒ���B�����s�̏ꍇ
		}
	}
    
    //��_�̌v�Z
	if( A2 != A1 && B2 != B1)//����A�A����B�����ɐ����łȂ��ꍇ
	{
		cx = ( Bb - Ab ) / ( Aa - Ba);
		cz = Aa * cx + Ab;
	}
    
	if( A2 == A1 )//����A�������̏ꍇ
	{
		cx= A1;
		cz = Ba * cx + Bb;
	}
    
	if( B2 == B1)//����B�������̏ꍇ
	{
		cx = B1;
		cz = Aa * cx + Ab;
	}
    
	Float ap[2];
	ap[0]=cx;
	ap[1]=cz;
    
	Vector nA=As-Ae;
	Vector nB=Bs-Be;
	Vector QnA=nA;
	QnA.Normalize();
	Vector QnB=nB;
	QnB.Normalize();
	
//	switch (data.GetLong(IDC_EC_PLANE))
//	{
//        case IDE_XZ:
//		{
			Float cyA=firstPoint(ap, QnA.y, As.y, QnA.x, Ae.z, nA.y, nA.z, nA.x, Ae.y, Ae.x);
			Float cyB=firstPoint(ap, QnB.y, Bs.y, QnB.x, Be.z, nB.y, nB.z, nB.x, Be.y, Be.x);
			*crossA = mg*Vector( cx, cyA, cz);
			*crossB = mg*Vector( cx, cyB, cz);
//		}
//            break;
//        case IDE_XY:
//		{
//			Real czA=firstPoint(ap, QnA.z, As.z, QnA.x, Ae.y, nA.z, nA.y, nA.x, Ae.z, Ae.x);
//			Real czB=firstPoint(ap, QnB.z, Bs.z, QnB.x, Be.y, nB.z, nB.y, nB.x, Be.z, Be.x);
//			*crossA = mg*Vector( cx, cz, czA );
//			*crossB = mg*Vector( cx, cz, czB);
//		}
//            break;
//        case IDE_YZ:
//		{
//			Real czA=firstPoint(ap, QnA.x, As.x, QnA.z, Ae.y, nA.x, nA.y, nA.z, Ae.x, Ae.z);
//			Real czB=firstPoint(ap, QnB.x, Bs.x, QnB.z, Be.y, nB.x, nB.y, nB.z, Be.x, Be.z);
//			*crossA = mg*Vector( czA, cz, cx);
//			*crossB = mg*Vector( czB, cz, cx);
//		}
//            break;
//	}
    
	
	return true;
}


//------------------------------------------------------------------------------
//���C���̌�_�|�C���g
Float DTOOL::firstPoint(Float *Gpos, Float A1, Float A2, Float A3, Float A4, Float A6, Float A7, Float A8, Float A9, Float A10)
{
	Float P1, L1, L2;
    
	if(A1==0){
		P1=A2;
	} else if(A3==0){
		L1=Gpos[1]-A4;
		P1=((L1*A6)/A7)+A9;
	}else{
		L2=Gpos[0]-A10;
		P1=((L2* A6)/A8)+A9;
	}
	return P1;
}



//*********************************************************************
//�}�E�X�|�C���g�̃��[���h�ʒu�����擾
//*********************************************************************
Vector DTOOL::SnapEdgeVector( BaseDocument *doc,
							 BaseContainer &data,
							 BaseDraw *bd, 
							 Float x,
							 Float y, 
							 Vector G_basePos,
							 Matrix G_mg, 
							 Int32 *G_SnapFlag,
							 Int32 G_STEP,
							 Vector G_markpoint,
							 Vector G_P2, 
							 Vector *G_nvec,
							 Matrix* SNmg, 
							 Vector* va,
							 Vector* vb)
{
	Vector MV=Vector();
	if(!doc) return MV;
	

	//�X�i�b�v�L��
	if(IsSnapEnabled(doc))
	{
	
		MV=bd->SW(Vector(x,y,500));
		if(doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
		{
			MV=bd->SW(Vector(x,y,-100000));
			//GePrint("jjjjJ");
		}

		AutoAlloc<SnapCore> snap;
		if (!snap) return MV;;

		if(!snap->Init(doc, bd)) return MV;

		Int32 view=NULL;
		view =	bd->GetProjection();
		SnapResult Snapres;

		//Vector nv=Vector(0,1,0);
		SNAPFLAGS sn=SNAPFLAGS_0;

		if(G_STEP > 1)
		{
			sn=SNAPFLAGS_IGNORE_SELECTED;
		}

	
		if (snap->Snap(MV, Snapres, sn ))
		{

			MV += Snapres.delta; //if pos can be snapped it is set accordingly

			
			if( IsSnapEnabled(doc, SNAPMODE_DYNAMICGUIDE))
			{
				if(Snapres.snapmode == SNAPMODE_DYNAMICGUIDE || Snapres.snapmode == SNAPMODE_ORTHO) 
				{
					Matrix Smg=Snapres.mat;
					Smg.off=MV;
					*SNmg=Smg;
                    *G_SnapFlag=1;
				}
			}
					
			
			if(SNAPMODE_WORKPLANE != Snapres.snapmode)*G_SnapFlag=4;
			else *G_SnapFlag=0;

			if(Snapres.snapmode== SNAPMODE_POINT) *G_SnapFlag=1;
			else if(Snapres.snapmode== SNAPMODE_POLYGON)*G_SnapFlag=2;


				Int32 PolyID=NULL;
				BaseObject *getOBJ = GetObjectFaceOBJ(doc, bd,data, x, y, &PolyID, true);


				if(getOBJ)
				{
					
					BaseObject* res=NULL;
					//�X�J���v�g�̃`�F�b�N
					if(getOBJ->GetTag(Tsculpt))
					{
						SculptTag* sctag=(SculptTag*)getOBJ->GetTag(Tsculpt);			
						SculptObject* Sc=sctag->GetSculptObject();
						if(Sc->IsFrozen())
						{
							Sc->SetFrozen(false);
						}
						Int32 sub=Sc->GetSubdivisionCount();
						//Fix���ꂽ�|���S�����擾
						PolygonObject* poly=Sc->GetPolygonCopy(sub,true); //�|���S�����R�s�[
						res=(BaseObject*)poly;
					}
					else
					{
						res=GetTargetObj( doc, getOBJ);
					}
					if(!res) return MV;
					PolygonObject *Ppoly=NULL;
					Ppoly=ToPoly(res);		
					if( Ppoly && Ppoly->GetPolygonCount() >0) //�f�t�H�[�}�[�`�F�b�N
					{
	
						if(Snapres.snapmode==SNAPMODE_EDGE || Snapres.snapmode==SNAPMODE_EDGEMID)
						{
							 
							AutoAlloc<ViewportSelect> vps;
							if(!vps) return MV;
							Int32 left, top, right, bottom, width, height;
							bd->GetFrame(&left, &top, &right, &bottom);
							width = right - left + 1;
							height = bottom - top + 1;
							Int32 Lx = (Int32)x;
							Int32 Ly = (Int32)y;
							Int32 rad=data.GetInt32(SNAP_RADIUSG);
							if(!vps->Init(width, height, bd, res, Medges, true, VIEWPORTSELECTFLAGS_USE_DEFORMERS | VIEWPORTSELECTFLAGS_IGNORE_HIDDEN_SEL)) return MV;
//	EditorWindow *win=bd->GetEditorWindow();
							vps->SetBrushRadius(rad);
							ViewportPixel* ep=vps->  GetNearestEdge(res, Lx, Ly, rad);

							if(ep)
							{

								const CPolygon*  poly = Ppoly->GetPolygonR(); //�|���S�����X�g�擾
								Int32 a, b, c, Id_e;
								a=NULL;
								b=NULL;
								c=NULL;
								//�G�b�W�C���f�b�N�X���擾
								Id_e = ep->i;
								//�G�b�W�ɂȂ���|�C���g�C���f�b�N�X�擾
								switch (Id_e % 4)
								{
									case 0: a=poly[Id_e/4].a; b=poly[Id_e/4].b; c=poly[Id_e/4].c;break;
									case 1: a=poly[Id_e/4].b; b=poly[Id_e/4].c; c=poly[Id_e/4].d;break;
									case 2: a=poly[Id_e/4].c; b=poly[Id_e/4].d; c=poly[Id_e/4].a;break;
									case 3: a=poly[Id_e/4].d; b=poly[Id_e/4].a; c=poly[Id_e/4].b;break;
								}

								PointObject* point=ToPoint(res);
								const Vector* vv=point->GetPointR();
								Matrix dmg= res->GetMg();
								*va = dmg*vv[a];
								*vb = dmg*vv[b];

								*G_SnapFlag=8;
							}
							vps->ClearPixelInfo((Int32)x, (Int32)y, VIEWPORT_CLEAR_POINT);
						}
					}
					else
					{
						*G_SnapFlag=0;
					}
					BaseObject::Free(res);
					res=NULL;
				}
				else
				{
					*G_SnapFlag=0;
				}
			//}		
		}
		else
		{
			*G_SnapFlag=0;
		}
		snap.Free();
	}
	else
	{
		*G_SnapFlag=0;
	}

//	GePrint("G_SnapFlag ="+Int32ToString(*G_SnapFlag));
	if(*G_SnapFlag==0)
	{
		MV=NonObjCursor(doc, bd, data, x, y, G_mg, G_STEP, G_markpoint, G_P2, G_nvec, false);
	}


	return MV;	
}

//Bool DTOOL::SetDPlane( BaseDraw* bd, BaseContainer &data,Bool flag, Int32 num)
//{
//	//���݂̃r���[���擾
//	
//
//	Int32 view =	bd->GetProjection();
//	if(view==6 || view==7) //top&bottom
//	{
//		//data.SetInt32(IDE_Snap_AXIS,IDE_XZ);
//		//BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		//Matrix mg;
//		//plane->SetMg(mg);
//		////G_mg=mg;
//	}
//	else if(view==4 || view==5) //front &back
//	{
//		//data.SetInt32(IDE_Snap_AXIS,IDE_XY);
//		//BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		//Matrix mg ;
//
//		// mg =MatrixRotX(Rad(-90.0));
//		// plane->SetMg(mg);
//		////G_mg=mg;
//	}
//	else if(view==3 || view==2) //right&left
//	{
//		//data.SetInt32(IDE_Snap_AXIS,IDE_YZ);
//		//BaseObject* plane= GetWorkplaneObject(GetActiveDocument());
//		//Matrix mg =MatrixRotZ(Rad(90.0));
//		//plane->SetMg(mg);
//	
//	}
// 	else
//	{
//		data.SetInt32(IDE_Snap_AXIS, IDE_XZ);
//		SetObjectPlane(GetActiveDocument(),data,flag);
//
//
//	}
//	return true;
//}

void DTOOL::GetLayerListdata(LayerObject *layer, Bool* flag,LayerObject ** Slayer)
{
    while(layer)
    {
        String name=layer->GetName();
        if(name == "Measure")
        {
            *Slayer=layer;
            *flag=true;
            return;
        }
    
        GetLayerListdata(layer->GetDown(), flag, Slayer);
        layer=layer->GetNext();
    }

}

void DTOOL::SerchiOBJ(BaseObject* op, BaseObject** tg)
{

     while(op)
    {
        String name=op->GetName();
        if(name == "##Measure_root##")
        {
            *tg=op;
            return;
        }
    
        SerchiOBJ(op->GetDown(), tg);
        op=op->GetNext();
    }
}


//Bool DTOOL::PointerToAXISEdgeCheck( BaseDraw* bd,BaseContainer &data, Vector* AP, Vector center, Int32* Num)
//{
//	//�X�N���[�����W�ɕϊ�
//	Vector c0=bd->WS(center);
//	c0.z=0;
//
//	Vector vp[3];
//	for(Int32 i=0; i< 3; i++)
//	{
//		vp[i]=bd->WS(AP[i]);
//		vp[i].z=0;
//	}
//
//
//
//	//Float r= data.GetInt32(	SNAP_RADIUSG); //�}�[�J�[���a
//	//printcheck( "c=", c0);
//	
//	for(Int32 i=0; i<3; i++)
//	{
//		//GePrint("i="+String::IntToString(i));
//		//printcheck( "vp[i]=", vp[i]-c0);
//
//		bd->DrawLine2D( c0,vp[i]); 
//
//		//Vector v= PointLineDistance(c0, vp[i]-c0, p);
//		//Float len = Len(v);
//		//if( len < r)
//		//{
//		//	
//		//	GePrint("len="+String::FloatToString(len));
//		//}
//
//	}
//	
//
//
//	return false;
//}



Bool DTOOL::PointerToAXISEdgeCheck( BaseDraw* bd,BaseContainer &data, Float x, Float y, Vector* AP, Vector center, Int32* Num)
{
	//���C���̎n�_���X�N���[�����W�ɕϊ�
	Vector pt1 = bd->WS(center);
	pt1.z = 0;


	//���C���̏I�_���X�N���[�����W�ɕϊ�
	Vector pt2[3];
	Vector vec[3];


	for (Int32 i = 0; i< 3; i++)
	{

		pt2[i] = bd->WS(AP[i]);

		pt2[i].z = 0;

		//�����x�N�g�����擾
		vec[i] = pt2[i] - pt1;
	}

	//�}�E�X���W
	Vector crcl = Vector(x, y, 0);

	Float r = data.GetInt32(SNAP_RADIUSG); //�}�[�J�[���a


	for (Int32 i = 0; i<3; i++)
	{
		//	GePrint("----------------"+String::IntToString(i)+"---------------");
		if (FuzzyPointInRange(data, bd, AP[i], x, y))
		{
			switch (i)
			{
			case 0:
				*Num = 100;
				//	GePrint("X");
				break;
			case 1:
				*Num = 200;
				//	GePrint("Y");
				break;
			case 2:
				*Num = 300;
				//	GePrint("Z");
				break;
			default:
				break;
			}
			return true;
		}
		else
		{
			Vector v = PointLineDistance(pt1, pt2[i] - pt1, crcl);

			if (Len(v) < r)
			{

				//���C����̍��W
				Vector v1 = crcl + v;

				//���C���̒��S�ʒu
				Vector va = (pt2[i] - pt1) / 2;

				va = va + pt1;

				Float len = Len(pt2[i] - pt1);

				if (Len(v1 - va) < len / 2)
				{

					switch (i)
					{
					case 0:
						*Num = 1;
						//	GePrint("X");
						break;
					case 1:
						*Num = 3;
						//	GePrint("Y");
						break;
					case 2:
						*Num = 7;
						//	GePrint("Z");
						break;
					default:
						break;
					}
				}
			}
		}
	}

	if (*Num == -1) return false;
	return true;
}






Bool DTOOL::ClingCheck(BaseDocument* doc, BaseObject* op, Int32 id)
{
	
	if (op == nullptr)
		return false;

	if (op->GetType() != Opolygon)
		return false;

	PolygonObject* poly = ToPoly(op);
	if (!poly)
		return false;

	Matrix mg = poly->GetMg();
	const Vector* points = poly->GetPointR();
	const CPolygon* polygons = poly->GetPolygonR();
	Vector normal = mg.TransformVector(CalcFaceNormal(points, polygons[id]));

	BaseDraw* bd = doc->GetActiveBaseDraw();

	Int32 a = polygons[id].a;
	//Int32 b = polygons[id].b;
//	Int32 c = polygons[id].c;
//Int32 d = polygons[id].d;

	Vector posa = mg * points[a];
//	Vector posb = mg * points[b];
//	Vector posc = mg * points[c];
//	Vector posd = mg * points[d];

	Bool bf = bd->BackfaceCulling(bd->WC_V(normal), bd->WC(posa));
	GePrint(String::IntToString(bf));

	return true;
}


//*********************************************************************
//�X�i�b�v��G�������g���̎擾
//*********************************************************************
Bool DTOOL::GetPolyErementInfo(BaseDocument* doc, BaseDraw *bd, BaseObject* getOBJ, SnapResult Snapres, Int32* G_SnapFlag, Float x, Float y, Matrix* G_mg)
{

	BaseObject* res = getOBJ;
	//�X�J���v�g�̃`�F�b�N
	//if (getOBJ->GetTag(Tsculpt))
	//{
	//	SculptTag* sctag = (SculptTag*)getOBJ->GetTag(Tsculpt);
	//	SculptObject* Sc = sctag->GetSculptObject();
	//	if (Sc->IsFrozen())
	//	{
	//		Sc->SetFrozen(false);
	//	}
	//	Int32 sub = Sc->GetSubdivisionCount();
	//	//Fix���ꂽ�|���S�����擾
	//	PolygonObject* poly = Sc->GetPolygonCopy(sub, true); //�|���S�����R�s�[
	//	res = (BaseObject*)poly;
	//}
	//else
	//{
	//	res = GetTargetObj(doc, getOBJ);
	//
	//}
	if (!res) return false;

	PolygonObject *Ppoly = NULL;
	Ppoly = ToPoly(res);

	if (Ppoly && Ppoly->GetPolygonCount() > 0) //�f�t�H�[�}�[�`�F�b�N
	{
		if (Snapres.snapmode == SNAPMODE_POLYGON || Snapres.snapmode == SNAPMODE_POLYGONCENTER) //�|���S���\�ʂ̍��W�Ɩ@���擾
		{
			//�R���W��������
			AutoAlloc<GeRayCollider> rc;
			if (!rc)
			{
				BaseObject::Free(res);
				//getOBJ = NULL;
				*G_SnapFlag = 0;
				return false;
			}
			GeRayColResult Fres;
			Vector wtail = bd->SW(Vector(x, y, 0));
			Vector whead = bd->SW(Vector(x, y, 20000000.0));
			Vector otail = (~res->GetMg()) * wtail;

			Vector oray = (~res->GetMg()).TransformVector(whead - wtail);

			rc->Init(Ppoly, true);
			rc->Intersect(otail, !oray, 20000000.0);
			if (rc->GetNearestIntersection(&Fres))
			{
				Matrix Amg = res->GetMg();

				//GePrint("res=" + res->GetName());

				//Vector n = MatrixToHPB(Amg, ROTATIONORDER_DEFAULT);
				//printcheckR("Amg r =", n);
				//printcheck("pos - ", Amg.off);

				Amg.off = Vector();
				Vector cp = Fres.s_normal;//�t�H���O�̖@���x�N�g��

				cp = Amg*cp;
				cp.Normalize();

				Vector Prot = VectorToHPB(cp); //�x�N�g������HPB�ɕϊ�
				Matrix lp = HPBToMatrix(Prot, ROTATIONORDER_DEFAULT); //HPB����}�g���b�N�X�ϊ�
				Matrix Mgx = MatrixRotX(Rad(-90.0)); //�X�O�x��]�}�g���b�N�X�쐬
				Matrix Low_Mg = lp*Mgx;  //�ϊ��}�g���b�N�X�łX�O�x��]
			
				//printcheck("Low_Mg=", Low_Mg.off);

				*G_mg = Low_Mg; //�@���}�g���b�N�X��S�̃}�g���b�N�X�ɐݒ�
				*G_SnapFlag = 2;

			}
		}
		else if (Snapres.snapmode == SNAPMODE_POINT) //�|�C���g�̍��W�Ɩ@���擾
		{

			Int32 p = Snapres.component;

			Neighbor nbr;
			PointObject* Pop = ToPoint(res);
			if (!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;
			Vector Pnv = GetVertexNormal(p, Ppoly, &nbr);
			Vector PR = VectorToHPB(Pnv);
			Matrix Mgx = MatrixRotX(Rad(-90.0));
			Matrix lp = HPBToMatrix(PR, ROTATIONORDER_DEFAULT);
			Matrix Low_Mg = lp*Mgx;
			*G_mg = Low_Mg;
			*G_SnapFlag = 1;
		}
		else if (Snapres.snapmode == SNAPMODE_EDGE || Snapres.snapmode == SNAPMODE_EDGEMID)
		{
			const CPolygon*  poly = Ppoly->GetPolygonR(); //�|���S�����X�g�擾
			Int32 a, b, c, Id_e;
			a = 0;
			b = 0;
			c = 0;
			//�G�b�W�C���f�b�N�X���擾
			//	Id_e = ep->i;
			Id_e = Snapres.component;
			//�G�b�W�ɂȂ���|�C���g�C���f�b�N�X�擾
			switch (Id_e % 4)
			{
			case 0: a = poly[Id_e / 4].a; b = poly[Id_e / 4].b; c = poly[Id_e / 4].c; break;
			case 1: a = poly[Id_e / 4].b; b = poly[Id_e / 4].c; c = poly[Id_e / 4].d; break;
			case 2: a = poly[Id_e / 4].c; b = poly[Id_e / 4].d; c = poly[Id_e / 4].a; break;
			case 3: a = poly[Id_e / 4].d; b = poly[Id_e / 4].a; c = poly[Id_e / 4].b; break;
			}

			Neighbor nbr;
			PointObject* Pop = ToPoint(res);
			if (!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;
			Vector PnvA = GetVertexNormal(a, Ppoly, &nbr);
			Vector PnvB = GetVertexNormal(b, Ppoly, &nbr);
			Vector Pnv = (PnvA + PnvB) / 2;
			Vector PR = VectorToHPB(Pnv);
			Matrix Mgx = MatrixRotX(Rad(-90.0));
			Matrix lp = HPBToMatrix(PR, ROTATIONORDER_DEFAULT);
			Matrix Low_Mg = lp*Mgx;
			*G_mg = Low_Mg;
			*G_SnapFlag = 3;

		}
	}
	else
	{
		*G_SnapFlag = 0;

	}

	//BaseObject::Free(res);
	//res = nullptr;


	return true;

}
Bool DTOOL::GetSnapFaceMatrix(BaseDocument* doc, BaseDraw* bd, Matrix* AxMg, Int32 id, BaseObject* op)
{
	if (op->GetType() != Opolygon) return false;
	Int32 ngonIdx = NOTOK;
	Pgon* ngonData = NULL;
	Matrix mg = op->GetMg();
	PolygonObject* poly = ToPoly(op);
	PointObject* pp = ToPoint(op);
	Vector* pv = pp->GetPointW();
	CPolygon* cp = poly->GetPolygonW();
	NgonBase* ngBase = poly->GetNgonBase();
	if (ngBase)
		ngonData = poly->GetAndBuildNgon();
	if (ngBase)
		ngonIdx = ngBase->FindPolygon(id);

	//Vector center=Vector();
	if (ngonIdx == NOTOK)
	{
		//�@���̎擾
		Vector nv = GetFaceNormal(pv, cp, id); //���[�J���@��
		//printcheck("nv=", nv);
		nv = mg*nv;
		Vector r = VectorToHPB(nv);

		//printcheckR("r=", r);
		Matrix m = HPBToMatrix(r, ROTATIONORDER_DEFAULT);
		Matrix rm = MatrixRotX(Rad(-90.0));
		m = m*rm;
		//�|���S���̒��S���W
		Vector cvp = GetFaceCenterPos(pv, cp, id);
		cvp = mg*cvp;
		m.off = cvp;
		*AxMg = m;
		return true;
	}
	return false;
}

//********************************************************************************************
//�^����ꂽ�|���S��ID����|���S���̖@����Ԃ�
//********************************************************************************************
Vector DTOOL::GetFaceNormal(Vector *vv, CPolygon *pArr, Int32 index)
{

	if (!vv) return Vector();
	Vector P[4];

	P[0] = vv[pArr[index].a];
	P[1] = vv[pArr[index].b];
	P[2] = vv[pArr[index].c];
	P[3] = vv[pArr[index].d];

	Vector v1;
	Vector v2;
	if (pArr[index].c == pArr[index].d)
	{
		v1 = P[1] - P[0];
		v2 = P[2] - P[0];
	}
	else
	{
		v1 = P[2] - P[0];
		v2 = P[3] - P[1];
	}
	//v1.Normalize();
	//v2.Normalize();
	Vector nv = Cross(v1, v2);
	nv.Normalize();
	return nv;
}

//********************************************************************************************
//�^����ꂽ�|���S��ID����|���S���̖@����Ԃ�
//********************************************************************************************
Vector DTOOL::GetFaceCenterPos(Vector *vv, CPolygon *pArr, Int32 index)
{

	if (!vv) return Vector();
	Vector P[4];

	P[0] = vv[pArr[index].a];
	P[1] = vv[pArr[index].b];
	P[2] = vv[pArr[index].c];
	P[3] = vv[pArr[index].d];

	Vector v1;
	Int32 n = 3;
	if (pArr[index].c != pArr[index].d)
	{
		n = 4;
	}

	for (Int32 i = 0; i < n; i++)
	{
		v1 = v1 + P[i];
	}
	v1 = v1 / n;
	return v1;
}

//******************************************************************************
//���_�@�������߂�
//******************************************************************************
Vector DTOOL::GetVertexNormal(Int32 PointIndex, PolygonObject *op, Neighbor *neighbor)
{
	// Variables
	CPolygon *pNeighborPoly;
	Int32 j, faceCnt, *pFaces = NULL;
	Vector v1, v2, vNorm = Vector(0, 1, 0);
	CPolygon *m_pPolys = op->GetPolygonW();
	Vector *m_pPoints = op->GetPointW();

	// Get polygons attached to point
	neighbor->GetPointPolys(PointIndex, &pFaces, &faceCnt);
	if (!faceCnt) return vNorm;

	for (j = 0; j<faceCnt; j++)
	{
		pNeighborPoly = &m_pPolys[pFaces[j]];

		// Compute face normal
		if (pNeighborPoly->c == pNeighborPoly->d)     //triangle
		{
			v1 = m_pPoints[pNeighborPoly->b] - m_pPoints[pNeighborPoly->a];
			v2 = m_pPoints[pNeighborPoly->c] - m_pPoints[pNeighborPoly->a];
		}
		else     //quadrangle
		{
			v1 = m_pPoints[pNeighborPoly->c] - m_pPoints[pNeighborPoly->a];
			v2 = m_pPoints[pNeighborPoly->d] - m_pPoints[pNeighborPoly->b];
		}

		//�Q�̃x�N�g���̊O�ς����߂�
		vNorm += Cross(v1, v2);
	}

	return !vNorm;
}



Matrix DTOOL::GetPlaneMatrix(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool PlaneSet_Flag, Matrix mg )
{
	Matrix GetMg;
	Int32 view = bd->GetProjection();
	BaseObject* plane = GetWorkplaneObject(doc);
	Matrix PlaneMg = plane->GetMg();


	//�I���\�r���[������world���[�h�̂Ƃ�
	if (data.GetBool(MADATA_SW_PLANEWORLD))//�������[���h���W�I��
	{
		GetMg = plane->GetMg();

		//���[�U�[��ƕ��ʂ��L���̎�
		if (PlaneSet_Flag ||  data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
		{
			Matrix mrp;
			if (data.GetInt32(IDE_Snap_AXIS) == IDE_XY)
			{
				 mrp = MatrixRotX(Rad(-90.0));
			}
			else if (data.GetInt32(IDE_Snap_AXIS) == IDE_YZ)
			{
				 mrp = MatrixRotZ(Rad(90.0));
			}
			else if (data.GetInt32(IDE_Snap_AXIS) == IDE_XZ)
			{
				mrp = Matrix();
			}
			GetMg = mrp * mg;
		}
		else
		{
			Matrix mrp;
			if (data.GetInt32(IDE_Snap_AXIS) == IDE_XY)
			{
				mrp = MatrixRotX(Rad(-90.0));
			}
			else if (data.GetInt32(IDE_Snap_AXIS) == IDE_YZ)
			{
				mrp = MatrixRotZ(Rad(90.0));
			}
			else if (data.GetInt32(IDE_Snap_AXIS) == IDE_XZ)
			{
				mrp = Matrix();
			}
			GetMg = mrp;

		}
		////�I���\�r���[�̎�

		//if (view == 4 || view == 5) //front &back
		//{
		//	Matrix mrp = MatrixRotX(Rad(-90.0));
		//	GetMg = PlaneMg *~mrp;

		//}
		//else if (view == 3 || view == 2) //right&left
		//{
		//	Matrix mrp = MatrixRotZ(Rad(90.0));
		//	GetMg = PlaneMg*~mrp;
		//}
		//else if (view == 6 || view == 7) //top&bottom
		//{

		//	Matrix mrp = Matrix();
		//	GetMg = PlaneMg *~mrp;
		//}
	}
	else //����ȊO
	{
		//GetMg = plane->GetMg();
		//�I���\�r���[�̎�
		Matrix mrp;
		if (view == 4 || view == 5) //front &back
		{
			if (data.GetInt32(IDE_Snap_AXIS) == IDE_XZ)
			{
				mrp = MatrixRotX(Rad(-90.0));
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_XY))
			{
				mrp = MatrixRotX(Rad(90.0));
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_YZ))
			{
				mrp = MatrixRotY(Rad(-90.0));
			}

		}
		else if (view == 3 || view == 2) //right&left
		{
			if (data.GetInt32(IDE_Snap_AXIS) == IDE_XZ)
			{
				mrp = MatrixRotZ(Rad(-90.0));
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_XY))
			{
				mrp = MatrixRotY(Rad(-90.0));
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_YZ))
			{
				mrp = MatrixRotZ(Rad( 90.0));
			}
			//GetMg = PlaneMg*mrp;
		}
		else if (view == 6 || view == 7) //top&bottom
		{
			if (data.GetInt32(IDE_Snap_AXIS) == IDE_XZ)
			{
				mrp = Matrix();
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_XY))
			{
				mrp = MatrixRotZ(Rad(-90.0));
			}
			else if ((data.GetInt32(IDE_Snap_AXIS) == IDE_YZ))
			{
				mrp = MatrixRotX(Rad(90.0));
			}
			
		}
		GetMg = mrp * PlaneMg;
		
	}
	//Vector r = MatrixToHPB(GetMg, ROTATIONORDER_DEFAULT);
	//printcheckR("GetMg Rot=", r);
	return GetMg;
}




/*
�}�E�X�J�[�\���̍��W��Ԃ�


*/
BaseObject* DTOOL::SnapMouseVectorObj(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, SnapCore*_snap, Float x, Float y, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag, Matrix* ViewMg, Float *mx, Float *my, Int32* SnapID, Vector& pos)
{


	if (!doc) return nullptr;
	//�r���[�^�C�v���擾



	Vector MV = bd->SW(Vector(x, y, 500));


	SnapResult	snapResul = SnapResult();
	SNAPFLAGS sn = SNAPFLAGS_0;

	//�V�F�[�f�B���O���[�h�̎擾
	GeData d;
	bd->GetParameter(BASEDRAW_DATA_SDISPLAYACTIVE, d, DESCFLAGS_GET_0);


	if (G_STEP >= 2)
	{
		//GePrint("!!!!!!!!!!!!!!!!");
		sn = SNAPFLAGS_IGNORE_SELECTED;
	}
	Bool checkFlag = false;
	*G_SnapFlag = 0;

	BaseObject* tg =nullptr;
	//�X�i�b�v���Ă�ꍇ
	if (_snap->Snap(MV, snapResul, sn))
	{

		MV = snapResul.mat.off;


		//�X�i�b�v�^�[�Q�b�g�̃I�u�W�F�N�g���擾
		BaseObject* _Starget = (BaseObject*)snapResul.target;
		checkFlag = true;

		//���C�����[�h�ȊO��visible�I�v�V�������L���̏ꍇ
		if (d.GetInt32() != 6 && data.GetBool(MDATA_VISIBLE))
		{
			if (snapResul.snapmode == SNAPMODE_POINT)
			{
				//���G�������g�̊m�F
				if (!CheckSnpPoint(bd, data, (BaseObject*)snapResul.target, x, y, SnapID))
				{
					checkFlag = false;//�X�i�b�v����
					BaseContainer bc = SnapSettings(doc); //�X�i�b�v�ݒ���擾
					bc.SetBool(SNAP_SETTINGS_ENABLED, false);
					SnapSettings(doc, bc);

				}
			}

			else if (snapResul.snapmode == SNAPMODE_EDGE)
			{
				Int32 s;
				if (!GetLineEdge(bd, data, (BaseObject*)snapResul.target, x, y, &s))
				{
					checkFlag = false;//�X�i�b�v����
					BaseContainer bc = SnapSettings(doc); //�X�i�b�v�ݒ���擾
					bc.SetBool(SNAP_SETTINGS_ENABLED, false);
					SnapSettings(doc, bc);
				}
			}
		}
		if (checkFlag)
		{
			*mx = x;
			*my = y;
			//�I���\�r���[���͂QD�X�i�b�v���W���擾
			if (doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
			{
				MV = snapResul.initial_snap;
			}

			//�X�i�b�v�^�[�Q�b�g�̏����擾
			tg = (BaseObject*)snapResul.target;

			_Starget=tg;
		//	GePrint(" _Starget = "+_Starget->GetName());
			//�X�i�b�v��̏����擾
			*SnapID = snapResul.component;

			//�X�i�b�v�^�C�v�̎擾
			if (snapResul.snapmode == SNAPMODE_POINT) *G_SnapFlag = 1;
			else if (snapResul.snapmode == SNAPMODE_POLYGON) *G_SnapFlag = 2;
			else if (snapResul.snapmode == SNAPMODE_MIDPOINT || snapResul.snapmode == SNAPMODE_POLYGONCENTER) *G_SnapFlag = 3;
			else if (snapResul.snapmode == SNAPMODE_EDGE) *G_SnapFlag = 4;


			//��ƕ��ʗp�G�������g�I�����[�h��
			if (PlaneSet_Flag )
			{
				if (snapResul.snapmode == SNAPMODE_POLYGON || snapResul.snapmode == SNAPMODE_POLYGONCENTER)
				{
					if (_Starget)//�X�i�b�v��I�u�W�F�N�g
					{
						//Matrix Omg;
						//�X�i�b�v��G�������g���̎擾
						if (G_STEP == 0)
						{
							//�|���S���\�ʂ̈ʒu��@���}�g���b�N�X���@ViewMg�ɑ��
							if (GetPolyErementInfo(doc, bd, _Starget, snapResul, G_SnapFlag, x, y, ViewMg))
							{
								Matrix m = *ViewMg;
								m.off = MV;
								*ViewMg = m;
							}
						}

					}
				}
			}
			//Vector CR = MatrixToHPB(*G_mg, ROTATIONORDER_DEFAULT);
			//printcheckR("othermg=", CR);
		}
	}
	else
	{
		//�X�i�b�v���Ă��Ȃ��ꍇ�փr���[��J�[�\�����W����
		*mx = x;
		*my = y;
	}

	if (*G_SnapFlag == 0 && G_STEP > 0)
	{
		MV = NonObjCursor(doc, bd, data, x, y, G_mg, G_STEP, G_markpoint, G_P2, G_nvec, PlaneSet_Flag);

	}

	pos=MV;
	return tg;
}


/*
�}�E�X�J�[�\���̍��W��Ԃ�


*/
Vector DTOOL::SnapMouseVector(BaseDocument *doc, BaseContainer &data, BaseDraw *bd, SnapCore*_snap, Float x, Float y, Matrix G_mg, Int32 *G_SnapFlag, Int32 G_STEP, Vector G_markpoint, Vector G_P2, Vector *G_nvec, Bool PlaneSet_Flag, Matrix* ViewMg, Float *mx, Float *my)
{


	if (!doc) return Vector();
	//�r���[�^�C�v���擾



	Vector MV = bd->SW(Vector(x, y, 500));


	SnapResult	snapResul = SnapResult();
	SNAPFLAGS sn = SNAPFLAGS_0;

	//�V�F�[�f�B���O���[�h�̎擾
	GeData d;
	bd->GetParameter(BASEDRAW_DATA_SDISPLAYACTIVE, d, DESCFLAGS_GET_0);


	if (G_STEP >= 2)
	{
		//GePrint("!!!!!!!!!!!!!!!!");
		sn = SNAPFLAGS_IGNORE_SELECTED;
	}
	Bool checkFlag = false;
	*G_SnapFlag = 0;


	//�X�i�b�v���Ă�ꍇ
	if (_snap->Snap(MV, snapResul, sn))
	{

		MV = snapResul.mat.off;


		//�X�i�b�v�^�[�Q�b�g�̃I�u�W�F�N�g���擾
		BaseObject* _Starget = (BaseObject*)snapResul.target;
		checkFlag = true;
		//�X�i�b�v��̏����擾
		Int32 SnapID = snapResul.component;
		//���C�����[�h�ȊO��visible�I�v�V�������L���̏ꍇ
		if (d.GetInt32() != 6 && data.GetBool(MDATA_VISIBLE))
		{
			if (snapResul.snapmode == SNAPMODE_POINT)
			{
				//���G�������g�̊m�F
				if (!CheckSnpPoint(bd, data, (BaseObject*)snapResul.target, x, y, &SnapID))
				{
					checkFlag = false;//�X�i�b�v����
					BaseContainer bc = SnapSettings(doc); //�X�i�b�v�ݒ���擾
					bc.SetBool(SNAP_SETTINGS_ENABLED, false);
					SnapSettings(doc, bc);

				}
			}

			else if (snapResul.snapmode == SNAPMODE_EDGE)
			{
				Int32 s;
				if (!GetLineEdge(bd, data, (BaseObject*)snapResul.target, x, y, &s))
				{
					checkFlag = false;//�X�i�b�v����
					BaseContainer bc = SnapSettings(doc); //�X�i�b�v�ݒ���擾
					bc.SetBool(SNAP_SETTINGS_ENABLED, false);
					SnapSettings(doc, bc);
				}
			}
		}
		if (checkFlag)
		{
			*mx = x;
			*my = y;
			//�I���\�r���[���͂QD�X�i�b�v���W���擾
			if (doc->GetActiveBaseDraw()->GetProjection() != CAMERA_PERSPECTIVE)
			{
				MV = snapResul.initial_snap;
			}

			//�X�i�b�v�^�C�v�̎擾
			if (snapResul.snapmode == SNAPMODE_POINT) *G_SnapFlag = 1;
			else if (snapResul.snapmode == SNAPMODE_POLYGON) *G_SnapFlag = 2;
			else if (snapResul.snapmode == SNAPMODE_MIDPOINT || snapResul.snapmode == SNAPMODE_POLYGONCENTER) *G_SnapFlag = 3;
			else if (snapResul.snapmode == SNAPMODE_EDGE) *G_SnapFlag = 4;


			//��ƕ��ʗp�G�������g�I�����[�h��
			if (PlaneSet_Flag )
			{
				if (snapResul.snapmode == SNAPMODE_POLYGON || snapResul.snapmode == SNAPMODE_POLYGONCENTER)
				{
					if (_Starget)//�X�i�b�v��I�u�W�F�N�g
					{
						//Matrix Omg;
						//�X�i�b�v��G�������g���̎擾
						if (G_STEP == 0)
						{
							//�|���S���\�ʂ̈ʒu��@���}�g���b�N�X���@ViewMg�ɑ��
							if (GetPolyErementInfo(doc, bd, _Starget, snapResul, G_SnapFlag, x, y, ViewMg))
							{
								Matrix m = *ViewMg;
								m.off = MV;
								*ViewMg = m;
							}
						}

					}
				}
			}
			//Vector CR = MatrixToHPB(*G_mg, ROTATIONORDER_DEFAULT);
			//printcheckR("othermg=", CR);
		}
	}
	else
	{
		//�X�i�b�v���Ă��Ȃ��ꍇ�փr���[��J�[�\�����W����
		*mx = x;
		*my = y;
	}

	if (*G_SnapFlag == 0 && G_STEP > 0)
	{
		MV = NonObjCursor(doc, bd, data, x, y, G_mg, G_STEP, G_markpoint, G_P2, G_nvec, PlaneSet_Flag);

	}
	return MV;
}

/*
����ʐݒ肨��у}�g���b�N�X��Ԃ�
	FaceMg:�|���S���@���̃}�g���b�N�X
	Pflag�F�C�ӕ��ʐݒ�̃t���O
*/
Matrix DTOOL::SetPlaneGetMg(BaseDocument* doc, BaseDraw* bd, BaseContainer& data, Bool Pflag, Matrix FaceMg, Int32 num)
{
	Matrix Gmg;
	if (data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc))
	{
	    Gmg = data.GetObjectLink(MDATA_DRAG_IDE_OBJ_TARGET, doc)->GetMg(); //�����N�I�u�W�F�N�g�̃}�g���b�N�X���擾
	}
	else if (Pflag)
	{
		Gmg = FaceMg;
	}

	Int32 view = bd->GetProjection();
	//����ʃI�u�W�F�N�g���擾
	BaseObject* plane = GetWorkplaneObject(doc);

	//�ݒ肵�Ă��镽�ʎ��^�C�v
	Int32 Ptype = data.GetInt32(IDE_Snap_AXIS);

    //�ݒ�ɂ���]�}�g���b�N�X���쐬
	Matrix r;
  	if (Ptype == IDE_XY)
	{
		r = MatrixRotX(Rad(-90.0));
	}
	else if (Ptype == IDE_YZ)
	{
		r = MatrixRotZ(Rad(90.0));
	}

	Vector p = Gmg.off;
	Gmg.off = Vector();


	Matrix mg = Gmg*r;

	mg.off= p;

	//����ʐݒ�
	plane->SetMg(mg);



	if(num ==1)
	{
		//�I���\�т�[�̏ꍇ�͊e���ʂ̃}�g���b�N�X��ϊ�
		if (view > 1 && view < 9)
		{
			Matrix mrp;
			if (view == 4 || view == 5) //front &back
			{
				mrp = MatrixRotX(Rad(-90.0));
	
			}
			else if (view == 3 || view == 2) //right&left
			{
				mrp = MatrixRotZ(Rad(-90.0));

			}
			mg = mg* mrp;
		}
	}

  //Vector CR = MatrixToHPB(  mg , ROTATIONORDER_DEFAULT);
  //  printcheckR("             mg=", CR);
	return mg;
}


Vector DTOOL::ReMgVector(BaseDraw* bd,  Matrix mg , Vector pos)
{
	Int32 view =	bd->GetProjection();
	Vector v=~mg * pos;
    if(view==6 || view==7) //top&bottom
    {
        v.y=0;
    }
    else if(view==4 || view==5 ) //front &back
    {
        v.z=0;
    }
    else if(view==3 ||view==2 ) //right&left
    {
        v.x+=0;
    }
	pos= mg*v;
	return pos;

}
//*********************************************************************
//���ʃO���b�h
//*********************************************************************
//Bool DTOOL ::SetGridPlane(BaseDocument *doc, BaseContainer &data, BaseDraw *bd,  BaseDrawHelp* bh, Matrix mg)
//{
//	if(!doc) return false;
//	bd->SetMatrix_Matrix(NULL, bh->GetMg());
//	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_ALWAYS);
//
//	//bd->SetDrawParam(DRAW_PARAMETER_ALPHA_THRESHOLD,0.0);
//
//	GeData gr=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SPACING);
//	GeData grsub=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_SUB);
//	GeData grsub_roug=bd->GetParameterData(BASEDRAW_DATA_SNAP_PLANE_ROUGHSUB);
//	Int32 gsize=gr.GetInt32(); //�O���b�h�P�̃T�C�Y
//	Int32 gsizesun=grsub.GetInt32();
//	Int32 gsizesun_roug=grsub_roug.GetInt32(); //���W���[�O���b�h���C���{��
//	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.0));
//	bd->SetDrawParam(DRAW_PARAMETER_SETZ,DRAW_Z_LOWEREQUAL);
//	bd->LineZOffset(0);
//
//	bd->SetPen(GetViewColor(VIEWCOLOR_GRID_MINOR));
//	Int32 va=gsize*(gsizesun/2); //�S�̂̔����̒���
//
//	for(Int32 i=0; i<gsizesun/2+1; i++)
//	{
//		Int32 sa=i*gsize;
//		Vector a=mg*Vector(sa,0,va);
//		Vector b=mg*Vector(sa,0,-va);
//		Vector c=mg*Vector(-sa,0,va);
//		Vector d=mg*Vector(-sa,0,-va);
//		bd->DrawLine(a,b,NOCLIP_Z);
//		bd->DrawLine(c,d, NOCLIP_Z);
//
//		a=mg*Vector(va,0,sa);
//		b=mg*Vector(-va,0,sa);
//		c=mg*Vector(va,0,-sa);
//		d=mg*Vector(-va,0,-sa);
//		bd->DrawLine(a,b, NOCLIP_Z);
//		bd->DrawLine(c,d, NOCLIP_Z);
//	}
//
//	//���W���[�O���b�h���C��
//	bd->SetDrawParam(DRAW_PARAMETER_LINEWIDTH, GeData(1.2));
//	bd->SetPen(GetViewColor(VIEWCOLOR_GRID_MAJOR));
//
//	Int32 Gl=va/(gsizesun_roug*gsize);
//
//	for(Int32 i=0; i<Gl+1; i++)
//	{
//		Int32 sa=i*gsizesun_roug*gsize;
//		Vector a=mg*Vector(sa,0,va);
//		Vector b=mg*Vector(sa,0,-va);
//		Vector c=mg*Vector(-sa,0,va);
//		Vector d=mg*Vector(-sa,0,-va);
//		bd->DrawLine(a,b, NOCLIP_Z);
//		bd->DrawLine(c,d, NOCLIP_Z);
//
//		a=mg*Vector(va,0,sa);
//		b=mg*Vector(-va,0,sa);
//		c=mg*Vector(va,0,-sa);
//		d=mg*Vector(-va,0,-sa);
//		bd->DrawLine(a,b, NOCLIP_Z);
//		bd->DrawLine(c,d, NOCLIP_Z);
//	}
//	return true;
//}


//*********************************************************************
//���_�@�������߂�
//*********************************************************************
//Vector DTOOL ::GetPointNormalVector( BaseObject *mo ,Int32 index)
//{
//	//if(!mo) return false;
//	//Int32 index = SnPoint_NUM;
//	//�|���S���I�u�W�F�N�g
//	PolygonObject* pPolyObj;
//	pPolyObj = ToPoly(mo);
//	//�|�C���g�I�u�W�F�N�g
//	PointObject *pPointObj;
//	pPointObj=ToPoint(mo);
//	//�|���S���z�񃊃X�g���擾
//	const CPolygon *pArr = NULL;
//	pArr = pPolyObj->GetPolygonW();
//	//�|���S�������擾
//	Int32 PolyCount = pPolyObj->GetPolygonCount();
//	Int32 count=0;
//	//�̈���m��
//	Int32 *carray =(Int32 *) GeAlloc(PolyCount * sizeof(Int32));
//	if(!carray)
//	{
//		GeFree(carray);
//		return false;
//	}
//
//	for(Int32 i=0; i<PolyCount; i++)
//	{
//		Int32 na[4]={pArr[i].a,pArr[i].b,pArr[i].c,pArr[i].d};
//		for(Int32 j=0; j<4; j++)
//		{
//			if(index == na[j])
//			{
//				carray[count]=i;
//				count += 1;
//			}
//		}
//	}
//	Vector Vnorm, v1, v2;
//	Vnorm=Vector();
//	const Vector *points=pPointObj->GetPointW();
//	for( Int32 i =0; i<count; i++ )
//	{
//		Int32 na[4]={pArr[carray[i]].a,pArr[carray[i]].b,pArr[carray[i]].c,pArr[carray[i]].d};
//		v1 = points[na[1]]-points[na[0]];
//
//		if(na[2] != na[3] )
//		{
//			v2 = points[na[3]]-points[na[0]];
//		}
//		else
//		{
//			v2 = points[na[2]]-points[na[0]];
//		}
//		Vnorm += v1 % v2; //�O�ς����߂�
//	}
//	GeFree(carray);
//	carray=NULL;
//	Vnorm /= count; //���ω�
//	Vnorm=!Vnorm; //�P�ʃx�N�g����
//	return Vnorm;
//}



//					Matrix mg=getOBJ->GetMg();
//					AutoAlloc<ViewportSelect> vps;
//					if(!vps) return MV;
//					Int32 left, top, right, bottom, width, height;
//					bd->GetFrame(&left, &top, &right, &bottom);
//					width = right - left + 1;
//					height = bottom - top + 1;
//					Int32 Lx = x;
//					Int32 Ly = y;
//					Int32 rad=data.GetInt32(SNAP_RADIUSG);
//					if(!vps->Init(width, height, bd, getOBJ, Mpolyedgepoint, true, VIEWPORTSELECTFLAGS_USE_DEFORMERS | VIEWPORTSELECTFLAGS_IGNORE_HIDDEN_SEL | VIEWPORTSELECTFLAGS_USE_HN)) return false;
//////	EditorWindow *win=bd->GetEditorWindow();
//					vps->SetBrushRadius(rad);
//					ViewportPixel* ep=vps-> GetNearestPoint(getOBJ, Lx, Ly, rad);
//					if(ep)
//					{
//						Int32 p;
//						p=ep->i;
//						Vector Pnv;
//						GePrint("p="+Int32ToString(p));
//						PolygonObject * Ppoly=ToPoly(getOBJ);
//						if(Snapres.snapmode==SNAPMODE_POLYGON ||Snapres.snapmode==SNAPMODE_POLYGONCENTER )
//						{
//						//	Neighbor nbr ;
//					//		PointObject* Pop=ToPoint(getOBJ);
//							Pnv=GetSurfaceNormal(p, Ppoly);
//							GePrint("kkkk");
//						}
//						else if(Snapres.snapmode==SNAPMODE_POINT)
//						{
//							Neighbor nbr ;
//							PointObject* Pop=ToPoint(getOBJ);
//							if(!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;
//							Pnv=GetVertexNormal(p, Ppoly, &nbr);
//						}
//						else if(Snapres.snapmode==SNAPMODE_EDGE || Snapres.snapmode==SNAPMODE_EDGEMID)
//						{
//							const CPolygon*  poly = Ppoly->GetPolygonR(); //�|���S�����X�g�擾
//							Int32 a, b, c, Id_e;
//							a=NULL;
//							b=NULL;
//							c=NULL;
//							//�G�b�W�C���f�b�N�X���擾
//							Id_e = ep->i;
//							//�G�b�W�ɂȂ���|�C���g�C���f�b�N�X�擾
//							switch (Id_e % 4)
//							{
//								case 0: a=poly[Id_e/4].a; b=poly[Id_e/4].b; c=poly[Id_e/4].c;break;
//								case 1: a=poly[Id_e/4].b; b=poly[Id_e/4].c; c=poly[Id_e/4].d;break;
//								case 2: a=poly[Id_e/4].c; b=poly[Id_e/4].d; c=poly[Id_e/4].a;break;
//								case 3: a=poly[Id_e/4].d; b=poly[Id_e/4].a; c=poly[Id_e/4].b;break;
//							}
//	//
//							Neighbor nbr ;
//							PointObject* Pop=ToPoint(getOBJ);
//							if(!nbr.Init(Pop->GetPointCount(), Ppoly->GetPolygonR(), Ppoly->GetPolygonCount(), NULL)) return false;
//							Vector PnvA=GetVertexNormal(a, Ppoly, &nbr);
//							Vector PnvB=GetVertexNormal(b, Ppoly, &nbr);
//							Pnv=(PnvA+PnvB)/2;
//						}
//						Vector PR=VectorToHPB(Pnv);
//						Matrix Mgx=MatrixRotX(Rad(-90.0)); 
//						Matrix lp=HPBToMatrix(PR, ROTATIONORDER_DEFAULT);
//						Matrix Low_Mg=lp*Mgx; 
//						*G_mg=Low_Mg;
//						*G_SnapFlag=2;
//					}
//					vps->ClearPixelInfo(x, y, VIEWPORT_CLEAR_POINT);
					
