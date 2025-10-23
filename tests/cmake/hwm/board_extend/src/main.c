/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * Copyright (c) 2025 Meta Platforms, Inc. and affiliates
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

#ifdef CONFIG_EXTENDED_VARIANT_BOARD_SETTING
#define EXTENDED_BOARD_A 1
#else
#define EXTENDED_BOARD_A 0
#endif

#ifdef CONFIG_EXTENDED_VARIANT_BOARD_ONE_SETTING_PROMPTLESS
#define EXTENDED_BOARD_ONE_B 1
#else
#define EXTENDED_BOARD_ONE_B 0
#endif

#ifdef CONFIG_EXTENDED_VARIANT_BOARD_TWO_SETTING_PROMPTLESS
#define EXTENDED_BOARD_TWO_C 1
#else
#define EXTENDED_BOARD_TWO_C 0
#endif

#ifdef CONFIG_EXTENDED_VARIANT_BOARD_SETTING_DEFCONFIG
#define EXTENDED_BOARD_D 1
#else
#define EXTENDED_BOARD_D 0
#endif

#ifdef CONFIG_BASE_BOARD_SETTING
#define BASE_BOARD_CONFIG 1
#else
#define BASE_BOARD_CONFIG 0
#endif

#ifdef CONFIG_SOC_MPS2_AN521_CPUTEST
#define EXTENDED_SOC 1
#else
#define EXTENDED_SOC 0
#endif

ZTEST_SUITE(soc_board_extend, NULL, NULL, NULL, NULL, NULL);

#if CONFIG_BOARD_NATIVE_SIM
ZTEST(soc_board_extend, test_native_sim_extend)
{
#if CONFIG_BOARD_NATIVE_SIM_NATIVE_ONE
	zassert_true(EXTENDED_BOARD_A, "Expected extended board to be set");
	zassert_true(EXTENDED_BOARD_ONE_B, "Expected extended board to be set");
	zassert_false(EXTENDED_BOARD_TWO_C, "Did not expect extended board two to be set");
	zassert_true(EXTENDED_BOARD_D, "Expected extended board to be set");
	zassert_false(BASE_BOARD_CONFIG, "Did not expect base board to be set");
	zassert_true(DT_NODE_EXISTS(DT_PATH(added_by_native_one)));
	zassert_false(DT_NODE_EXISTS(DT_PATH(added_by_native_two)));
	zassert_false(DT_NODE_EXISTS(DT_PATH(adc)));
#elif CONFIG_BOARD_NATIVE_SIM_NATIVE_64_TWO
	zassert_true(EXTENDED_BOARD_A, "Expected extended board to be set");
	zassert_false(EXTENDED_BOARD_ONE_B, "Did not expect extended board one to be set");
	zassert_true(EXTENDED_BOARD_TWO_C, "Expected extended board to be set");
	zassert_true(EXTENDED_BOARD_D, "Expected extended board to be set");
	zassert_false(BASE_BOARD_CONFIG, "Did not expect base board to be set");
	zassert_false(DT_NODE_EXISTS(DT_PATH(added_by_native_one)));
	zassert_true(DT_NODE_EXISTS(DT_PATH(added_by_native_two)));
	zassert_false(DT_NODE_EXISTS(DT_PATH(adc)));
#else
	zassert_true(false, "Did not expect to build for a base native_sim board");
#endif
#elif CONFIG_BOARD_MPS2
ZTEST(soc_board_extend, test_an521_soc_extend)
{
#if CONFIG_BOARD_MPS2_AN521_CPUTEST
	zassert_true(EXTENDED_SOC, "Expected extended SoC to be set");
#elif CONFIG_BOARD_MPS2
	zassert_true(false, "Did not expect to build for a base mps2 board");
#endif

#else
ZTEST(soc_board_extend, test_failure)
{
	zassert_true(false, "Did not expect to build for a regular board");
#endif
}

#if CONFIG_BOARD_NATIVE_SIM_NATIVE_64_TWO
ZTEST(soc_board_extend, test_qualifier_matching)
{
	// My interpretation of the documentation [here](https://docs.zephyrproject.org/latest/hardware/porting/board_porting.html#write-kconfig-files)
	// is that the the defconfig files should be matched with native_sim_defconfig, as well as each file getting more specific with each qualifier
	// (e.g. native_sim_defconfig, native_sim_native_defconfig, native_sim_native_64_defconfig, native_sim_native_64_two_defconfig)
	//
	// > If both the common plank_defconfig file and one or more board qualifiers specific plank_<qualifiers>_defconfig files exist, then all matching files will be used.
	// > This allows you to place configuration which is common for all board SoCs, CPU clusters, and board variants in the base plank_defconfig and only place the
	// > adjustments specific for a given SoC or board variant in the plank_<qualifiers>_defconfig.
	//
	// Depending on your interpretation of the documentation, the you may have a different opinion on
	// which configs with "intermediate" qualifiers should be included... However, I would consistently
	// expect `CONFIG_SET_BY_NATIVE_SIM` to be true regardless of interpretation of docs.

	// success
	zexpect_false(IS_ENABLED(CONFIG_SET_BY_NATIVE));

	// failure - native_sim_defconfig is surprisingly not used
	// "This allows you to place configuration which is common for all board SoCs, CPU clusters, and board variants in the base plank_defconfig"
	zexpect_true(IS_ENABLED(CONFIG_SET_BY_NATIVE_SIM));

	// failure - native_sim_native_defconfig is surprisingly not used
	zexpect_true(IS_ENABLED(CONFIG_SET_BY_NATIVE_SIM_NATIVE));

	// failure - native_sim_native_64_defconfig is surprisingly not used
	zexpect_true(IS_ENABLED(CONFIG_SET_BY_NATIVE_SIM_NATIVE_64));

	// success - but surprised that *only* the most specific defconfig file is used
	zexpect_true(IS_ENABLED(CONFIG_SET_BY_NATIVE_SIM_NATIVE_64_TWO));

	// success
	zexpect_false(IS_ENABLED(CONFIG_SET_BY_NATIVE_SIM_NATIVE_32_TWO));

	// success
	zexpect_false(IS_ENABLED(CONFIG_SET_BY_NATIVE_SI)); // intentionally missing 'M'
}
#endif
