
#ifndef __gupnp_marshal_MARSHAL_H__
#define __gupnp_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:STRING,POINTER (./gupnp-marshal.list:1) */
extern void gupnp_marshal_VOID__STRING_POINTER (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* VOID:POINTER,POINTER (./gupnp-marshal.list:2) */
extern void gupnp_marshal_VOID__POINTER_POINTER (GClosure     *closure,
                                                 GValue       *return_value,
                                                 guint         n_param_values,
                                                 const GValue *param_values,
                                                 gpointer      invocation_hint,
                                                 gpointer      marshal_data);

G_END_DECLS

#endif /* __gupnp_marshal_MARSHAL_H__ */

