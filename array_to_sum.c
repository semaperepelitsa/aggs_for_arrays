Datum array_to_sum(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(array_to_sum);

/**
 * Returns a sum from an array of numbers.
 * by Paul A. Jungwirth
 */
Datum
array_to_sum(PG_FUNCTION_ARGS)
{
  // Our arguments:
  ArrayType *vals;

  // The array element type:
  Oid valsType;

  // The array element type widths for our input array:
  int16 valsTypeWidth;

  // The array element type "is passed by value" flags (not really used):
  bool valsTypeByValue;

  // The array element type alignment codes (not really used):
  char valsTypeAlignmentCode;

  // The array contents, as PostgreSQL "Datum" objects:
  Datum *valsContent;

  // List of "is null" flags for the array contents (not used):
  bool *valsNullFlags;

  // The size of the input array:
  int valsLength;

  float8 v = 0;
  int i;

  if (PG_ARGISNULL(0)) {
    ereport(ERROR, (errmsg("Null arrays not accepted")));
  }

  vals = PG_GETARG_ARRAYTYPE_P(0);

  if (ARR_NDIM(vals) == 0) {
    PG_RETURN_NULL();
  }
  if (ARR_NDIM(vals) > 1) {
    ereport(ERROR, (errmsg("One-dimesional arrays are required")));
  }

  if (array_contains_nulls(vals)) {
    ereport(ERROR, (errmsg("Array contains null elements")));
  }

  // Determine the array element types.
  valsType = ARR_ELEMTYPE(vals);

  if (valsType != INT2OID &&
      valsType != INT4OID &&
      valsType != INT8OID &&
      valsType != FLOAT4OID &&
      valsType != FLOAT8OID) {
    ereport(ERROR, (errmsg("Sum subject must be SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION values")));
  }

  valsLength = (ARR_DIMS(vals))[0];

  if (valsLength == 0) PG_RETURN_NULL();

  get_typlenbyvalalign(valsType, &valsTypeWidth, &valsTypeByValue, &valsTypeAlignmentCode);

  // Extract the array contents (as Datum objects).
  deconstruct_array(vals, valsType, valsTypeWidth, valsTypeByValue, valsTypeAlignmentCode,
&valsContent, &valsNullFlags, &valsLength);

  // Iterate through the contents and sum things up
  switch (valsType) {
    case INT2OID:
      for (i = 0; i < valsLength; i++) {
        v += (DatumGetInt16(valsContent[i]));
      }
      break;
    case INT4OID:
      for (i = 0; i < valsLength; i++) {
        v += (DatumGetInt32(valsContent[i]));
      }
      break;
    case INT8OID:
      for (i = 0; i < valsLength; i++) {
        v += (DatumGetInt64(valsContent[i]));
      }
      break;
    case FLOAT4OID:
      for (i = 0; i < valsLength; i++) {
        v += (DatumGetFloat4(valsContent[i]));
      }
      break;
    case FLOAT8OID:
      for (i = 0; i < valsLength; i++) {
        v += (DatumGetFloat8(valsContent[i]));
      }
      break;
    default:
      ereport(ERROR, (errmsg("Sum subject must be SMALLINT, INTEGER, BIGINT, REAL, or DOUBLE PRECISION values")));
      break;
  }
  PG_RETURN_FLOAT8(v);
}

