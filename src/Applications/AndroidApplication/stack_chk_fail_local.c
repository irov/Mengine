extern void __stack_chk_fail();

__attribute__ ((visibility ("hidden")))
void __stack_chk_fail_local (void)
{
    __stack_chk_fail();
}