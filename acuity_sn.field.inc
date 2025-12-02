<?php

/**
 * File: acuity_sn.field.inc
 */
function acuity_sn_field_widget_info() {
  return [
    'acuity_sn_widget' => [
      'label' => t('Auto serial number'),
      'field types' => ['text'],
      'settings' => ['group' => ''],
    ],
  ];
}

/**
 * Implements hook_field_widget_form().
 */
function acuity_sn_field_widget_form($form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $group_name = $instance['widget']['settings']['group'] ?? '';
  $preview = '';

  // Check if this is a new entity
  $is_new = isset($form['#node']) && empty($form['#node']->nid);

  if (!empty($group_name) && $is_new) {
    $config = config('acuity_sn.serials');
    $all = $config->get();

    if (isset($all[$group_name])) {
      $next = acuity_sn_peek_next_number($group_name);
      $preview = acuity_sn_format_number($all[$group_name], $next);
    }
  }
  else {
    // Use existing field value
    $preview = $items[$delta]['value'] ?? '';
  }

  // Main serial number field (read-only)
  $element['value'] = [
    '#type' => 'textfield',
    '#title' => $instance['label'],
    '#default_value' => $preview,
    '#attributes' => ['readonly' => 'readonly', 'style' => 'color: red; font-style: italic;'],
  ];

  if ($is_new) {
    $element['value']['#description'] = '<span class="abms-sn-provisional-msg">' . t('This is a provisional number and may change once this has been saved.') . '</span>';
  }

  // Hidden backup field to preserve value during edit
  $element['preserve'] = [
    '#type' => 'hidden',
    '#value' => $preview,
  ];

  return $element;
}

/**
 * Implements hook_field_settings_form().
 */
function acuity_sn_field_settings_form($field, $instance, $has_data) {
  $settings = $field['settings']['acuity_sn'] ?? [];

  $form['group'] = [
    '#type' => 'textfield',
    '#title' => t('Serial group name'),
    '#default_value' => $settings['group'] ?? '',
    '#required' => TRUE,
  ];

  return $form;
}

/**
 * Implements hook_field_widget_settings_form().
 */
function acuity_sn_field_widget_settings_form($field, $instance) {
  $config = config('acuity_sn.serials')->get();
  $options = [];

  foreach ($config as $key => $group) {
    $options[$key] = $group['label'] . " ($key)";
  }

  $form['group'] = [
    '#type' => 'select',
    '#title' => t('Serial number group'),
    '#options' => $options,
    '#default_value' => $instance['widget']['settings']['group'] ?? '',
    '#required' => TRUE,
    '#description' => t('Select which serial number group this field will use.'),
  ];

  return $form;
}