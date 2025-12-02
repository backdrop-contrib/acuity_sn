<?php

/**
 * @file
 * Hooks provided by the Acuity Serial Number Generator module.
 *
 * file: acuity_sn.api.inc
 */

/**
 * RECOMMENDED INTEGRATION PATTERN:
 *
 * 1. Check existing groups:
 *    $existing = acuity_sn_discover_groups();
 *
 * 2. Implement hook_acuity_sn_info() to define your group(s)
 *
 * 3. Call acuity_sn_register_discovered_groups() to persist them
 */

/**
 * Declare serial number groups provided by your module.
 *
 * Modules can implement this hook to define one or more serial number groups
 * that should be auto-registered in the Acuity Serial Number system.
 *
 * @return array
 *   An associative array of group_id => settings. Each settings array should
 *   include:
 *   - label: (optional) Human-readable label.
 *   - prefix: (optional) String prefix.
 *   - suffix: (optional) String suffix.
 *   - padding: (int) Minimum number of digits (default 4).
 *   - next: (int) Next number to use (default 1).
 *
 * Declare serial number groups provided by your module.
 *
 * This is an example hook implementation that can be placed in any custom
 * module to define serial number groups.
 *
 * @return array
 *   An associative array of group_id => settings.
 */
function hook_acuity_sn_info() {
  return [
    'group_test' => [
      'label' => 'Test Serial Group',
      'prefix' => 'TEST-',
      'suffix' => '',
      'padding' => 4,
      'next' => 1000,
    ],
  ];
}

/**
 * Load all serial groups defined by other modules via hook_acuity_sn_info().
 *
 * @return array
 *   An associative array of group_id => settings arrays.
 */
function acuity_sn_discover_groups() {
  $groups = [];
  foreach (module_implements('acuity_sn_info') as $module) {
    $callback = $module . '_acuity_sn_info';
    $module_groups = $callback();
    if (is_array($module_groups)) {
      foreach ($module_groups as $group_id => $settings) {
        // Prefix with module name to avoid clashes? (optional)
        $groups[$group_id] = $settings;
      }
    }
  }
  return $groups;
}

/**
 * Register any serial number groups defined by other modules via hook_acuity_sn_info().
 *
 * This function will loop through all module-defined groups and save them to
 * the acuity_sn.serials config if they do not already exist.
 *
 * @return array
 *   A list of group_ids that were added.
 */
function acuity_sn_register_discovered_groups() {
  $added = [];
  $defined = acuity_sn_discover_groups();
  $config = config('acuity_sn.serials');
  $existing = $config->get() ?: [];

  foreach ($defined as $group_id => $settings) {
    if (!isset($existing[$group_id])) {
      // Add a label if missing for safety
      $settings += ['label' => $group_id];
      $existing[$group_id] = [
        'label' => $settings['label'],
        'prefix' => $settings['prefix'] ?? '',
        'suffix' => $settings['suffix'] ?? '',
        'padding' => (int) ($settings['padding'] ?? 4),
        'next' => (int) ($settings['next'] ?? 1),
      ];
      $added[] = $group_id;
    }
  }

  if (!empty($added)) {
    $config->setData($existing)->save();
  }

  return $added;
}
