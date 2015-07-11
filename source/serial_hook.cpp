
#include "c4d_resource.h"
#include "lib_sn.h"
#include "c4d_symbols.h"
#include "c4d_general.h"
#define ID_SNAPPRS_SN 1029476 
#define ID_MODELING_SnapALIGN 1024956

#define sn_size 16
class SerialHookClass : public SNHookClass
{
    public:
        SerialHookClass()
        {
			name = GeLoadString(IDS_SNAPPRS)+GeLoadString(IDC_VER);
			
            if(!name.Content())
                name = "C++ SDK - Example Serial Hook";

            sn_ok = false;
        }
		
		virtual ~SerialHookClass() { }

        virtual Int32 SNCheck(const String &c4dsn,const String &sn,Int32 regdate,Int32 curdate)
        {
			if( !regdate && !sn.Content() )
			return SN_WRONGNUMBER;

			Int32 type=0;

			//c4dsn C4Dのシリアルバンバーからキーコードを生成
			String namber=GetNumber(type);

			String plug_sn;
			if(type==1) //マルチライセンスの場合
			{
				Int32 dashPos;
				if(sn.FindFirst("-", &dashPos,0))
				{
					plug_sn = sn.SubStr(dashPos+1, sn.GetLength()-dashPos-1);
				}

			} else {
				plug_sn=sn;
			}

			Char *Cstr;
			Cstr = NewMemClear(Char, sn_size);
			if(!Cstr)
			{
				sn_ok = false;
				return SN_WRONGNUMBER;
			}

           if(plug_sn.Content() && plug_sn == namber)
            {
                sn_ok = true;

				namber.GetCString(Cstr,sn_size);
				if( type == 1){
					WriteRegInfo(ID_MODELING_SnapALIGN,Cstr,sn_size);
				}
				else
				{
					WritePluginInfo(ID_MODELING_SnapALIGN,Cstr,sn_size);
				}
				DeleteMem(Cstr);
                return SN_OKAY;
            }


			namber="000000000000000";
			namber.GetCString(Cstr,sn_size);
			if( type == 1){
				WriteRegInfo(ID_MODELING_SnapALIGN,Cstr,sn_size);
			}
			else
			{
				WritePluginInfo(ID_MODELING_SnapALIGN,Cstr,sn_size);
			}
			DeleteMem(Cstr);
            sn_ok = false;
            return SN_WRONGNUMBER;
        }

        virtual const String& GetTitle()
        {
            return name;
        }

        Bool IsSerialOK() const { return sn_ok; }

		//CINEMA4Dのシリアルナンバーを取得しpluginシリアルを返す
		String GetNumber( Int32 &ID)
		{
			String sn0 = GetLicens(ID );
			String sn1;
			sn1=sn0.SubStr(0,10); // "fgh"

			Int32 Lsn=sn1.ToInt32();
			//GePrint("Lsn="+LongToString(Lsn));
			//C4Dシリアルからpluginシリアル生成
			sn1=GetPSN(Lsn);
			return sn1;

		}

		//C4dシリアルからプラグインシリアル生成
		String GetPSN(const Int32 csn)
		{
			// Float a=9387528534;
			//return geneSN( csn, a, "KF", "dkt");

			// Float a=5067745768;
			//return geneSN( csn, a, "TH", "qdj");

			Float a = 22689325865;
			return geneSN(csn, a, "Fw", "Afh");


			//return s1;
		}

		String geneSN( Int32 sn, Float ch, String s1, String s2)
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

		String GetLicens(Int32 &ID )
		{

 			String c4d_sn;
			SerialInfo si;
			GeGetSerialInfo(SERIALINFO_MULTILICENSE, &si);

			if(si.nr.Content())
			{
				// if you need to find the license count, you can do it like so...
				//Int32 licCount = si.nr.SubStr(3,3).ToLong(NULL);

				c4d_sn = si.nr;

				ID=1;
			}  else {

				GeGetSerialInfo(SERIALINFO_CINEMA4D,&si);
				c4d_sn = si.nr;

				ID=2;
			}

			return 	c4d_sn;
		}
    private:
        String name;
        Bool sn_ok;
};

SerialHookClass *snhook = nullptr;

Bool RegisterSerialHook()
{
    snhook = NewObjClear(SerialHookClass);
    if (!snhook->Register(ID_SNAPPRS_SN, SNFLAG_OWN))
        return false;
    return true;
}

void FreeSNHook()
{
    if (snhook)
        DeleteObj(snhook);
}

Bool IsSerialOKHook()
{
    if (snhook)
        return snhook->IsSerialOK();

    return false;
}