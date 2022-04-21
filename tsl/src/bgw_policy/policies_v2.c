/*
 * This file and its contents are licensed under the Timescale License.
 * Please see the included NOTICE for copyright information and
 * LICENSE-TIMESCALE for a copy of the license.
 */

#include <postgres.h>
#include <access/xact.h>
#include <miscadmin.h>
#include <utils/builtins.h>

#include "compression_api.h"
#include "errors.h"
#include "hypertable.h"
#include "hypertable_cache.h"
#include "policy_utils.h"
#include "utils.h"
#include "jsonb_utils.h"
#include "bgw/job.h"
#include "bgw_policy/job.h"
#include "bgw_policy/continuous_aggregate_api.h"
#include "bgw_policy/policies_v2.h"

Datum
policies_add(PG_FUNCTION_ARGS)
{
    Oid cagg_oid, start_offset_type, end_offset_type;
    Interval refresh_interval;
    bool if_exists;
    Datum start_offset, end_offset;

    cagg_oid = PG_GETARG_OID(0);
    if_exists = PG_GETARG_BOOL(1);

    if (PG_ARGISNULL(2) || PG_ARGISNULL(3) || PG_ARGISNULL(4))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("cannot use NULL refresh_start_offset, or refresh_end_offset or refresh_schedule_interval")));

    start_offset_type = get_fn_expr_argtype(fcinfo->flinfo, 2);
	start_offset = PG_GETARG_DATUM(2);
	end_offset_type = get_fn_expr_argtype(fcinfo->flinfo, 3);
	end_offset = PG_GETARG_DATUM(3);
    refresh_interval = *PG_GETARG_INTERVAL_P(4);

    policy_refresh_cagg_add_internal(cagg_oid, start_offset_type, start_offset, end_offset_type,
                                     end_offset, refresh_interval, if_exists);

    if (!PG_ARGISNULL(5))
    {
        Datum compress_after_datum = PG_GETARG_DATUM(5);
	    Oid compress_after_type = get_fn_expr_argtype(fcinfo->flinfo, 5);
        policy_compression_add_internal(cagg_oid, compress_after_datum, compress_after_type, if_exists);
    }
	if (!PG_ARGISNULL(6))
    {
        Datum drop_after_datum = PG_GETARG_DATUM(6);
	    Oid drop_after_type = get_fn_expr_argtype(fcinfo->flinfo, 6);
        policy_retention_add_internal(cagg_oid, drop_after_datum, drop_after_type, if_exists);
    }
    PG_RETURN_VOID();
}

Datum
policies_remove(PG_FUNCTION_ARGS)
{
    Oid cagg_oid = PG_GETARG_OID(0);
    bool if_exists = PG_GETARG_BOOL(1);
    ArrayType *policy_names = PG_GETARG_ARRAYTYPE_P(2);
    ArrayIterator at;
    bool isnull;
    Datum datum;

    at = array_create_iterator(policy_names, 0, NULL);
    while (array_iterate(at, &datum, &isnull))
    {
        char *curr_policy = DatumGetCString(datum);

        if (pg_strcasecmp(curr_policy, POLICY_REFRESH_CAGG_PROC_NAME) == 0)
                policy_refresh_cagg_remove_internal(cagg_oid, if_exists);
        else if (pg_strcasecmp(curr_policy, POLICY_COMPRESSION_PROC_NAME) == 0)
                policy_compression_remove_internal(cagg_oid, if_exists);
        else if (pg_strcasecmp(curr_policy, POLICY_RETENTION_PROC_NAME) == 0)
                policy_retention_remove_internal(cagg_oid, if_exists);
        else
            ereport(NOTICE,
                (errmsg("No relevant policy found")));
    }
    PG_RETURN_VOID();
   
}

Datum
policies_alter(PG_FUNCTION_ARGS)
{
    // How to decide which policies to alter and pass relevant arguments only to the functions below 

    // Remove old policies
    
    // Add new policies 
    PG_RETURN_VOID();
}

Datum
policies_show(PG_FUNCTION_ARGS)
{
    PG_RETURN_VOID();
 
}