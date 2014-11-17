#include "m_pd.h"
#include "math.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ war~ ----------------------------- */



static t_class *war_class;

typedef struct warctl
{
    double c_Resources;
    double c_Iprev;
    double c_Aprev;
    double c_Bprev;
    double c_inflow;
    double c_assetA;
    double c_assetB;
    double c_DefenseA;
    double c_DefenseB;
    double c_DA;
    double c_DB;
    double c_DMA;
    double c_DMB;
    double c_DT;
    double c_k0;
    double c_k1;
    double c_k2;
    double c_k3;
    double c_k4;
    double c_k5;
    double c_k6;
    double c_k7;
    double c_k8;
    double c_k9;
    double c_k10;
    double c_k11;
    double c_k12;
    double c_k13;
    double c_k14;
    double c_k15;
    double c_k16;
    double c_k17;
} t_warctl;

typedef struct _war
{
    t_object x_obj; 	/* obligatory header */
    double x_I;    	/* place to hold inlet's value if it's set by message */
    t_warctl x_cspace;
    t_warctl *x_ctl;
} t_war;

t_int *war_perform(t_int *w)
{
    t_float *inI = (t_float *)(w[1]);
    t_float *outP = (t_float *)(w[2]);
    t_float *outA = (t_float *)(w[3]);
    t_float *outB = (t_float *)(w[4]);
    t_warctl *c =   (t_warctl *)(w[5]);
    int n = (int)(w[6]);
    while (n--)
    {
       double inflow = (double)(*(inI));
       // double dasset_a = *(inA) - c->c_Aprev;
       // double dasset_b = *(inB) - c->c_Bprev;

    	double asset_a = c->c_assetA;
    	double asset_b = c->c_assetB;

    	//inflow += dinflow;
    	//asset_a += dasset_a;
    	//asset_b += dasset_b;

    	c->c_Resources = inflow / (1+ (c->c_k0*asset_a) + (c->c_k1*asset_b));

        if(c->c_Resources < 0)
            c->c_Resources = 0;

        c->c_DA = (c->c_k2*c->c_Resources*asset_a) - (c->c_k3*asset_a) - (c->c_k6*asset_a) - (c->c_k13*asset_a*c->c_DefenseB);
        c->c_DB = (c->c_k4*c->c_Resources*asset_b) - (c->c_k5*asset_b) - (c->c_k9*asset_b) - (c->c_k12*asset_b*c->c_DefenseA);
        c->c_DMA = (c->c_k7*asset_a) - (c->c_k8*c->c_DefenseA) - (c->c_k16*c->c_DefenseA*asset_b) - (c->c_k15*c->c_DefenseA*c->c_DefenseB);
        c->c_DMB = (c->c_k10*asset_b) - (c->c_k11*c->c_DefenseB) - (c->c_k17*c->c_DefenseB*c->c_DefenseA) - (c->c_k14*asset_a*c->c_DefenseB);

        asset_a = asset_a + (c->c_DA*c->c_DT);
        asset_b = asset_b + (c->c_DB*c->c_DT);

        c->c_DefenseA = c->c_DefenseA + (c->c_DMA*c->c_DT);
        c->c_DefenseB = c->c_DefenseB + (c->c_DMB*c->c_DT);



    	*(outP) = (t_float)((c->c_k2*c->c_Resources*asset_a) + (c->c_k4*c->c_Resources*asset_b));
//    	*(outP) = *(outP) < 1e+36 ? 0 : *(outP);
    	*(outA) = (t_float)asset_a;
//    	*(outA) = *(outA) < 1e+36 ? 0 : *(outA);
    	*(outB) = (t_float)asset_b;
//    	*(outB) = *(outB) < 1e+36 ? 0 : *(outB);


        c->c_Iprev = (double)*(inI++);
       // c->c_Aprev = *(inA++);
       // c->c_Bprev = *(inB++);

    	//c->c_inflow = *(outP++);
    	*(outP++);
    	c->c_assetA = (double)*(outA++);
    	c->c_assetB = (double)*(outB++);

    }
    return (w+7);
}

static void war_dsp(t_war *x, t_signal **sp)
{
    dsp_add(war_perform, 6, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, x->x_ctl, sp[0]->s_n);
}

void war_mfas(t_war *x)
{

    x->x_ctl->c_DA = 0.0f;
    x->x_ctl->c_DB = 0.0f;
    x->x_ctl->c_DMA = 0.0f;
    x->x_ctl->c_DMB = 0.0f;

    x->x_ctl->c_inflow = 1.f;
    x->x_ctl->c_assetA = 0.1f;
    x->x_ctl->c_assetB = 0.1f;
    x->x_ctl->c_Iprev = 0.0f;
    x->x_ctl->c_Aprev = 0.0f;
    x->x_ctl->c_Bprev = 0.0f;
    x->x_ctl->c_DT = 0.001;
    x->x_ctl->c_DefenseA = 0;
    x->x_ctl->c_DefenseB = 0;

    //coefficients
    x->x_ctl->c_k0 = 4;
    x->x_ctl->c_k1 = 4;
    x->x_ctl->c_k2 = 0.5;
    x->x_ctl->c_k3 = 0.1;
    x->x_ctl->c_k4 = 0.5;
    x->x_ctl->c_k5 = 0.1;
    x->x_ctl->c_k6 = 0.011;
    x->x_ctl->c_k7 = 0.055;
    x->x_ctl->c_k8 = 0.05;
    x->x_ctl->c_k9 = 0.01;
    x->x_ctl->c_k10 = 0.05;
    x->x_ctl->c_k11 = 0.05;
    x->x_ctl->c_k12 = 0.1;
    x->x_ctl->c_k13 = 0.1;
    x->x_ctl->c_k14 = 0.01;
    x->x_ctl->c_k15 = 0.01;
    x->x_ctl->c_k16 = 0.01;
    x->x_ctl->c_k17 = 0.01;
    post("war: RATZZ!");
}

void war_DT(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DT = (double)g;
    post("DT: %f", g);
}

void war_k0(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k0 = (double)g;
    post("k0: %f", g);
}
void war_k1(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k1 = (double)g;
    post("k1: %f", g);
}

void war_k2(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k2 = (double)g;
    post("k2: %f", g);
}

void war_k3(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k3 = (double)g;
    post("k3: %f", g);
}

void war_k4(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k4 = (double)g;
    post("k4: %f", g);
}

void war_k5(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k5 = (double)g;
    post("k5: %f", g);
}

void war_k6(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k6 = (double)g;
    post("k6: %f", g);
}

void war_k7(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k7 = (double)g;
    post("k7: %f", g);
}

void war_k8(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k8 = (double)g;
    post("k8: %f", g);
}

void war_k9(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k9 = (double)g;
    post("k9: %f", g);
}
void war_k10(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k10 = (double)g;
    post("k10: %f", g);
}

void war_k11(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k11 = (double)g;
    post("k11: %f", g);
}

void war_k12(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k12 = (double)g;
    post("k12: %f", g);
}

void war_k13(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k13 = (double)g;
    post("k13: %f", g);
}

void war_k14(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k14 = (double)g;
    post("k14: %f", g);
}

void war_k15(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k15 = (double)g;
    post("k15: %f", g);
}

void war_k16(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k16 = (double)g;
    post("k16: %f", g);
}

void war_k17(t_war *x, t_floatarg g)
{
    x->x_ctl->c_k17 = (double)g;
    post("k17: %f", g);
}

void war_assetA(t_war *x, t_floatarg g)
{
    x->x_ctl->c_assetA = (double)g;
    post("assetA: %f", g);
}

void war_assetB(t_war *x, t_floatarg g)
{
    x->x_ctl->c_assetB = (double)g;
    post("assetB: %f", g);
}

void war_DefenseA(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DefenseA = (double)g;
    post("DefenseA: %f", g);
}

void war_DefenseB(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DefenseB = (double)g;
    post("DefenseB: %f", g);
}

void war_DA(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DA = (double)g;
    post("DA: %f", g);
}

void war_DB(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DB = (double)g;
    post("DB: %f", g);
}

void war_DMA(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DMA = (double)g;
    post("DMA: %f", g);
}

void war_DMB(t_war *x, t_floatarg g)
{
    x->x_ctl->c_DMB = (double)g;
    post("DMB: %f", g);
}

static void *war_new(void)
{
        t_war *x = (t_war *)pd_new(war_class);
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DT"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k0"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k1"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k2"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k3"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k4"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k5"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k6"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k7"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k8"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k9"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k10"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k11"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k12"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k13"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k14"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k15"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k16"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("k17"));


        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("assetA"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("assetB"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DefenseA"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DefenseB"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DA"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DB"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DMA"));
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("DMB"));

        outlet_new(&x->x_obj, gensym("signal"));
        outlet_new(&x->x_obj, gensym("signal"));
        outlet_new(&x->x_obj, gensym("signal"));
        x->x_I = 0;
        x->x_ctl = &x->x_cspace;
        x->x_cspace.c_k0 = 0.0f;
        x->x_cspace.c_k1 = 0.0f;
        return (x);
}

void war_tilde_setup(void)
{
    war_class = class_new(gensym("war~"), (t_newmethod)war_new, 0,
    	sizeof(t_war), 0, A_GIMME, 0);
    class_addmethod(war_class, (t_method)war_mfas, gensym("mfas"), 0);
    class_addmethod(war_class, (t_method)war_DT, gensym("DT"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k0, gensym("k0"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k1, gensym("k1"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k2, gensym("k2"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k3, gensym("k3"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k4, gensym("k4"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k5, gensym("k5"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k6, gensym("k6"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k7, gensym("k7"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k8, gensym("k8"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k9, gensym("k9"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k10, gensym("k10"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k11, gensym("k11"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k12, gensym("k12"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k13, gensym("k13"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k14, gensym("k14"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k15, gensym("k15"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k16, gensym("k16"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_k17, gensym("k17"), A_FLOAT, 0);

    class_addmethod(war_class, (t_method)war_assetA, gensym("assetA"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_assetB, gensym("assetB"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DefenseA, gensym("DefenseA"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DefenseB, gensym("DefenseB"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DA, gensym("DA"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DB, gensym("DB"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DMA, gensym("DMA"), A_FLOAT, 0);
    class_addmethod(war_class, (t_method)war_DMB, gensym("DMB"), A_FLOAT, 0);


    CLASS_MAINSIGNALIN(war_class, t_war, x_I);

    class_addmethod(war_class, (t_method)war_dsp, gensym("dsp"), 0);
}

