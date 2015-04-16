/* Return the initial module search path. */

#ifdef __cplusplus
 extern "C" {
#endif

/* External interface */

char *
Py_GetPath(void)
{
    return "";
}

char *
Py_GetPrefix(void)
{
    return "";
}

char *
Py_GetExecPrefix(void)
{
    return "";
}

char *
Py_GetProgramFullPath(void)
{
    return "";
}


#ifdef __cplusplus
}
#endif

