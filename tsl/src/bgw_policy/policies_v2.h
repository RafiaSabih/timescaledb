/*
 * This file and its contents are licensed under the Timescale License.
 * Please see the included NOTICE for copyright information and
 * LICENSE-TIMESCALE for a copy of the license.
 */

#include <postgres.h>
#include <utils/jsonb.h>
#include "dimension.h"
#include <continuous_aggs/materialize.h>
#include <bgw_policy/compression_api.h>
#include <bgw_policy/continuous_aggregate_api.h>
#include <bgw_policy/retention_api.h>

#define POLICY_REFRESH_CAGG_PROC_NAME "policy_refresh_continuous_aggregate"
#define POLICY_COMPRESSION_PROC_NAME "policy_compression"
#define POLICY_RETENTION_PROC_NAME "policy_retention"

extern Datum policies_add(PG_FUNCTION_ARGS);
extern Datum policies_remove(PG_FUNCTION_ARGS);
extern Datum policies_alter(PG_FUNCTION_ARGS);
extern Datum policies_show(PG_FUNCTION_ARGS);
