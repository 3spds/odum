#include "m_pd.h"
#include "math.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ tank~ ----------------------------- */



static t_class *tank_class;

typedef struct tankctl
{
    t_float c_curr;
    t_float c_leak;
} t_tankctl;

typedef struct _tank
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    t_tankctl x_cspace;
    t_tankctl *x_ctl;
//    t_float x_leak;     /* leak variable (inlet variable) */
//    float curr;

} t_tank;

    /* this is the actual performance routine which acts on the samples.
    It's called with a single pointer "w" which is our location in the
    DSP call list.  We return a new "w" which will point to the next item
    after us.  Meanwhile, w[0] is just a pointer to dsp-perform itself
    (no use to us), w[1] and w[2] are the input and output vector locations,
    and w[3] is the number of points to calculate. */
t_int *tank_perform(t_int *w)
{
    t_float *in1 = (t_float *)(w[1]);
    t_float *in2 = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);
    t_tankctl *c =   (t_tankctl *)(w[4]);
    int n = (int)(w[5]);

    while (n--)
    {
    	t_float j = *(in1++); //grab input vector #1
    	t_float l = *(in2++); //grab input vector 2
//    	c->c_curr = (c->c_curr + (j - (l * c->c_curr)));
    	*out = j + l*c->c_curr;
    	c->c_curr = *out++;
//    	float dq = (j - (leak * curr)); //calculate change in q
//        curr = curr + dq; //apply change
        //c->c_curr; //send to output vector
//        *out++ = j;
    }
    return (w+6);
}

//void tank_leak(t_tank *x, t_floatarg g)
//{
//    x->x_leak = g;
//    post("leak: %f", g);
//}
    /* called to start DSP.  Here we call Pd back to add our perform
    routine to a linear callback list which Pd in turn calls to grind
    out the samples. */
static void tank_dsp(t_tank *x, t_signal **sp)
{
    dsp_add(tank_perform, 5, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, x->x_ctl, sp[0]->s_n);
}


static void *tank_new(void)
{
        t_tank *x = (t_tank *)pd_new(tank_class);
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("signal"), gensym("signal"));
        outlet_new(&x->x_obj, gensym("signal"));
        x->x_f = 0;
        x->x_ctl = &x->x_cspace;
        x->x_cspace.c_curr = 0.0f;
        x->x_cspace.c_leak = 0.0f;
        return (x);
}

    /* this routine, which must have exactly this name (with the "~" replaced
    by "_tilde) is called when the code is first loaded, and tells Pd how
    to build the "class". */
void tank_tilde_setup(void)
{
    tank_class = class_new(gensym("tank~"), (t_newmethod)tank_new, 0,
    	sizeof(t_tank), 0, A_GIMME, 0);

    CLASS_MAINSIGNALIN(tank_class, t_tank, x_f);
    	/* here we tell Pd about the "dsp" method, which is called back
	when DSP is turned on. */
    class_addmethod(tank_class, (t_method)tank_dsp, gensym("dsp"), 0);
}
