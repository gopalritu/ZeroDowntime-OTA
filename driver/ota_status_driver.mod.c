#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xf5a4e43d, "misc_register" },
	{ 0xe8213e80, "_printk" },
	{ 0x0e9cab28, "memset" },
	{ 0x9aa6980d, "mutex_lock" },
	{ 0x40a621c5, "snprintf" },
	{ 0x9aa6980d, "mutex_unlock" },
	{ 0x437e81c7, "simple_read_from_buffer" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x3c568d08, "misc_deregister" },
	{ 0xaa47b76e, "__arch_copy_to_user" },
	{ 0xaa47b76e, "__arch_copy_from_user" },
	{ 0x9c4ed43a, "alt_cb_patch_nops" },
	{ 0xe54e0a6b, "__fortify_panic" },
	{ 0xc7ea9460, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xf5a4e43d,
	0xe8213e80,
	0x0e9cab28,
	0x9aa6980d,
	0x40a621c5,
	0x9aa6980d,
	0x437e81c7,
	0xd272d446,
	0x3c568d08,
	0xaa47b76e,
	0xaa47b76e,
	0x9c4ed43a,
	0xe54e0a6b,
	0xc7ea9460,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"misc_register\0"
	"_printk\0"
	"memset\0"
	"mutex_lock\0"
	"snprintf\0"
	"mutex_unlock\0"
	"simple_read_from_buffer\0"
	"__stack_chk_fail\0"
	"misc_deregister\0"
	"__arch_copy_to_user\0"
	"__arch_copy_from_user\0"
	"alt_cb_patch_nops\0"
	"__fortify_panic\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "AA969D02C5712BCD21DD701");
