<?php

/**
 * @file
 * Acuity Serial Number Generator admin interface.
 *
 * file: acuity_sn.admin.inc
 */

/**
 * Implements hook_help_info().
 */
function acuity_sn_help_info() {
  return [
    'acuity_sn' => [
      'title' => 'Acuity Serial Number Generator',
      'path' => 'admin/config/acuity-utils/acuity_sn/help',
    ],
  ];
}

/**
 * Implements hook_help_section().
 */
function acuity_sn_help_section($section) {
  if ($section === 'admin/help#acuity') {
    return [
      'acuity_sn' => [
        'title' => 'Acuity Serial Number Generator',
        'path' => 'admin/config/acuity-utils/acuity_sn/help',
      ],
    ];
  }
}

/**
 * Admin settings form for Acuity Serial Number Generator.
 */
function acuity_sn_admin_form($form, &$form_state) {
  // --- Serial number groups section ---
  $form['groups_wrapper'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['acuity-sn-admin-section', 'acuity-sn-groups']],
  ];

  $form['groups_wrapper']['groups_description'] = [
    '#markup' => '<h3>' . t('Serial number groups') . '</h3>' .
                 '<p>' . t('These are your current serial number groups.') . '</p>',
  ];

  $config = config('acuity_sn.serials');
  $groups = $config->get();

  $header = [
    t('Edit'),
    t('Group ID'),
    t('Label'),
    t('Prefix'),
    t('Suffix'),
    t('Padding'),
    t('Next Number'),
    t('Delete'),
  ];

  $rows = [];
  if (!empty($groups)) {
    foreach ($groups as $group_id => $group) {
      $rows[] = [
        l(t('Edit'), "admin/config/acuity-utils/acuity_sn/edit/$group_id", [
          'attributes' => ['title' => t('Edit @group', ['@group' => $group_id])],
        ]),
        check_plain($group_id),
        check_plain($group['label'] ?? ''),
        check_plain($group['prefix'] ?? ''),
        check_plain($group['suffix'] ?? ''),
        check_plain($group['padding'] ?? ''),
        check_plain($group['next'] ?? ''),
        l(t('Delete'), "admin/config/acuity-utils/acuity_sn/delete/$group_id", [
          'attributes' => ['class' => ['abms-delete-link']],
        ]),
      ];
    }
  }

  $form['groups_wrapper']['groups_table'] = [
    '#markup' => theme('table', ['header' => $header, 'rows' => $rows]),
  ];

  $form['groups_wrapper']['add'] = [
    '#type' => 'link',
    '#title' => t('Add new group'),
    '#href' => 'admin/config/acuity-utils/acuity_sn/add',
    '#attributes' => ['class' => ['button', 'button-action']],
  ];

  // --- Title field assignments section ---
  $form['title_assignments_wrapper'] = [
    '#type' => 'container',
    '#attributes' => ['class' => ['acuity-sn-admin-section', 'acuity-sn-title-assignments']],
  ];

  $form['title_assignments_wrapper']['description'] = [
    '#markup' => '<h3>' . t('Title field assignments') . '</h3>' .
                 '<p>' . t('These content types will have their Title field automatically populated with a serial number from the selected group.') . '</p>',
  ];

  $assignments = config_get('acuity_sn.title_assignment', 'assignments');
  $header = [t('Content Type'), t('Serial Number Group')];
  $rows = [];

  if (!empty($assignments) && is_array($assignments)) {
    foreach ($assignments as $type => $group) {
      $rows[] = [
        l(t('Edit'), "admin/config/acuity-utils/acuity_sn/edit-title/$type", [
          'attributes' => ['title' => t('Edit title assignment for @type', ['@type' => $type])],
        ]),
        check_plain(node_type_get_name($type)),
        check_plain($group),
        l(t('Remove'), "admin/config/acuity-utils/acuity_sn/remove-title/$type", [
          'attributes' => ['class' => ['abms-remove-link']],
        ]),
      ];
    }

    $form['title_assignments_wrapper']['table'] = [
      '#markup' => theme('table', ['header' => [t('Edit'), t('Content Type'), t('Group ID'), t('Remove')], 'rows' => $rows]),
    ];
  }
  else {
    $form['title_assignments_wrapper']['empty'] = [
      '#markup' => '<p><em>' . t('No title assignments yet.') . '</em></p>',
    ];
  }

  $form['title_assignments_wrapper']['add'] = [
    '#type' => 'markup',
    '#markup' => l(t('Assign serial number to title'), 'admin/config/acuity-utils/acuity_sn/assign-title', [
      'attributes' => ['class' => ['button', 'button-primary']],
    ]),
  ];

  return $form;
}

/**
 * Form to add or edit a serial number group.
 *
 * @param array $form
 * @param array $form_state
 * @param string|null $group_id
 */
function acuity_sn_group_form($form, &$form_state, $group_id = NULL) {
  $config = config('acuity_sn.serials');
  $groups = $config->get() ?: [];

  $group = $group_id && isset($groups[$group_id]) ? $groups[$group_id] : [];

  // Store ID for later
  $form['group_id'] = [
    '#type' => 'machine_name',
    '#title' => t('Group ID'),
    '#default_value' => $group_id,
    '#disabled' => !empty($group_id),
    '#required' => TRUE,
    '#description' => t('A machine readable field name that must consist of lowercase letters, numbers and underscores but it must start with a character. i.e. "invoice_number", "po_number", "widget_serial".'),
    '#machine_name' => [
      'exists' => function ($id) use ($groups) {
        return isset($groups[$id]);
      },
    ],
  ];

  $form['label'] = [
    '#type' => 'textfield',
    '#title' => t('Label'),
    '#default_value' => $group['label'] ?? '',
    '#required' => TRUE,
    '#description' => t('This is the human readable text and field label used on forms and when selecting the widget from the Content Type field settings.'),
  ];

  $form['prefix'] = [
    '#type' => 'textfield',
    '#title' => t('Prefix'),
    '#default_value' => $group['prefix'] ?? '',
    '#description' => t('Automatically added to the beginning of the serial number i.e. "Inv00125".'),
  ];

  $form['suffix'] = [
    '#type' => 'textfield',
    '#title' => t('Suffix'),
    '#default_value' => $group['suffix'] ?? '',
    '#description' => t('Automatically appended to the end of the serial number i.e. "Inv00125pending".'),
  ];

  $form['padding'] = [
    '#type' => 'number',
    '#title' => t('Zero Padding'),
    '#default_value' => $group['padding'] ?? 4,
    '#min' => 1,
    '#description' => t('Zero padding adds a 0 to the left of the serial number i.e. 4 = 0001, 5 = 00001.'),
  ];

  $form['next'] = [
    '#type' => 'number',
    '#title' => t('Next Number'),
    '#default_value' => $group['next'] ?? 1,
    '#required' => TRUE,
    '#description' => t('Next serial number to use. This is also used for the starting number. This should be numeric with no padding i.e. "1".'),
  ];

  $form['actions'] = [
    '#type' => 'actions',
  ];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => $group_id ? t('Save changes') : t('Add group'),
  ];

  return $form;
}

function acuity_sn_group_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $config = config('acuity_sn.serials');
  $groups = $config->get() ?: [];

  $id = $values['group_id'];

  $groups[$id] = [
    'label' => $values['label'],
    'prefix' => $values['prefix'],
    'suffix' => $values['suffix'],
    'padding' => (int) $values['padding'],
    'next' => (int) $values['next'],
  ];

  $config->setData($groups)->save();

  backdrop_set_message(t('Serial number group %id has been saved.', ['%id' => $id]));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
}

/**
 * Confirmation form for deleting a serial number group.
 */
function acuity_sn_group_delete_confirm($form, &$form_state, $group_id = NULL) {
  $config = config('acuity_sn.serials');
  $groups = $config->get();

  if (empty($group_id) || !isset($groups[$group_id])) {
    backdrop_not_found();
    return;
  }

  $form['group_id'] = [
    '#type' => 'value',
    '#value' => $group_id,
  ];

  $usage = acuity_sn_group_usage_details($group_id);

  if (!empty($usage)) {
    $form['warning'] = [
      '#markup' => '<div class="messages warning">' .
        check_plain(t('This group is currently in use and cannot be deleted.')) .
        '</div>',
    ];

    $rows = [];
    foreach ($usage as $item) {
      $rows[] = [
        check_plain($item['entity_type']),
        check_plain($item['bundle']),
        check_plain($item['field_name']),
      ];
    }

    $form['usage_table'] = [
      '#type' => 'markup',
      '#markup' => theme('table', [
        'header' => [t('Entity Type'), t('Bundle'), t('Field Name')],
        'rows' => $rows,
      ]),
    ];
    $form['back_link'] = [
      '#type' => 'markup',
      '#markup' => '<p>' . l(
        t('← Back to Serial Number Settings'),
        'admin/config/acuity-utils/acuity_sn',
        ['attributes' => ['class' => ['button', 'button-secondary']]]
      ) . '</p>',
    ];
  }

  else {
    $form = confirm_form(
      $form,
      t('Are you sure you want to delete the serial number group %id?', ['%id' => $group_id]),
      'admin/config/acuity-utils/acuity_sn',
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
    );
  }

  return $form;
}

/**
 * Submit handler for serial group delete confirmation.
 */
function acuity_sn_group_delete_confirm_submit($form, &$form_state) {
  $group_id = $form_state['values']['group_id'];

  // Safety check: is the group still in use?
  if (acuity_sn_group_usage_details($group_id)) {
    backdrop_set_message(t('The group %id is now in use and cannot be deleted.', ['%id' => $group_id]), 'error');
    $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
    return;
  }

  // Proceed with deletion
  $config = config('acuity_sn.serials');
  $groups = $config->get();

  if (isset($groups[$group_id])) {
    unset($groups[$group_id]);
    $config->setData($groups)->save();

    backdrop_set_message(t('Serial number group %id has been deleted.', ['%id' => $group_id]));
  }
  else {
    backdrop_set_message(t('The group %id was not found.', ['%id' => $group_id]), 'error');
  }

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
}

/**
 * Find all fields using a specific serial number group.
 *
 * @param string $group_id
 * @return array
 * An array of arrays with keys: entity_type, bundle, field_name.
 */
function acuity_sn_group_usage_details($group_id) {
  $usage = [];

  $config_path = config_get_config_directory('active');
  $files = file_scan_directory($config_path, '/^field\.instance\..*\.json$/');

  foreach ($files as $file) {
    $data = json_decode(file_get_contents($file->uri), TRUE);
    if (
      isset($data['widget']['settings']['group']) &&
      $data['widget']['settings']['group'] === $group_id
    ) {
      $usage[] = [
        'entity_type' => $data['entity_type'],
        'bundle' => $data['bundle'],
        'field_name' => $data['field_name'],
      ];
    }
  }

  return $usage;
}

/**
 * Form for assigning a serial number group to a content type's title field.
 */
function acuity_sn_assign_title_form($form, &$form_state) {
  // Load existing assignments.
  $assignments = config_get('acuity_sn.title_assignment', 'assignments') ?? [];

  // Get all content types.
  $all_types = node_type_get_types();
  $available_types = [];

  foreach ($all_types as $type => $info) {
    if (!isset($assignments[$type])) {
      $available_types[$type] = $info->name;
    }
  }

  if (empty($available_types)) {
    backdrop_set_message(t('All content types are already assigned.'), 'warning');
    backdrop_goto('admin/config/acuity-utils/acuity_sn');
  }

  // Load serial number groups.
  $serial_groups = config_get('acuity_sn.serials') ?? [];
  $group_options = [];

  foreach ($serial_groups as $group_id => $settings) {
    $group_options[$group_id] = $settings['label'] . ' (' . $group_id . ')';
  }

  $form['content_type'] = [
    '#type' => 'select',
    '#title' => t('Content type'),
    '#options' => $available_types,
    '#required' => TRUE,
  ];

  $form['serial_group'] = [
    '#type' => 'select',
    '#title' => t('Serial number group'),
    '#options' => $group_options,
    '#required' => TRUE,
  ];

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => t('Save assignment'),
    '#button_type' => 'primary',
  ];

  return $form;
}

/**
 * Form submit handler for acuity_sn_assign_title_form().
 */
function acuity_sn_assign_title_form_submit($form, &$form_state) {
  $content_type = $form_state['values']['content_type'];
  $group_id = $form_state['values']['serial_group'];

  $config = config('acuity_sn.title_assignment');
  $assignments = $config->get('assignments') ?? [];

  $assignments[$content_type] = $group_id;
  $config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Serial number group %group assigned to content type %type.', [
    '%group' => $group_id,
    '%type' => $content_type,
  ]));

  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
}

/**
 * Form to edit a title field serial number assignment.
 *
 * @param string $type
 * The machine name of the content type.
 */
function acuity_sn_edit_title_assignment_form($form, &$form_state, $type) {
  $assignments = config_get('acuity_sn.title_assignment', 'assignments');
  $current_group = $assignments[$type] ?? '';

  // Load serial groups.
  $groups = config_get('acuity_sn.serials');

  $options = [];
  foreach ($groups as $group_id => $group) {
    $label = $group['label'] ?? $group_id;
    $options[$group_id] = check_plain($label);
  }

  $form['type'] = [
    '#type' => 'value',
    '#value' => $type,
  ];

  $form['group'] = [
    '#type' => 'select',
    '#title' => t('Serial number group'),
    '#options' => $options,
    '#default_value' => $current_group,
    '#required' => TRUE,
  ];

$form['actions'] = [
  '#type' => 'actions',
];

$form['actions']['submit'] = [
  '#type' => 'submit',
  '#value' => t('Save assignment'),
  '#button_type' => 'submit',
];

$form['actions']['cancel'] = [
  '#type' => 'link',
  '#title' => t('Cancel'),
  '#href' => 'admin/config/acuity-utils/acuity_sn',
  '#attributes' => ['class' => ['button', 'button-secondary']],
];

  return $form;
}

/**
 * Submit handler for acuity_sn_edit_title_assignment_form().
 */
function acuity_sn_edit_title_assignment_form_submit($form, &$form_state) {
  $type = $form_state['values']['type'];
  $group = $form_state['values']['group'];

  $config = config('acuity_sn.title_assignment');
  $assignments = $config->get('assignments') ?? [];
  $assignments[$type] = $group;
  $config->set('assignments', $assignments)->save();

  backdrop_set_message(t('The title assignment for %type has been updated.', ['%type' => $type]));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
}

/**
 * Form callback to remove a title assignment for a content type.
 */
function acuity_sn_remove_title_assignment_form($form, &$form_state, $content_type = NULL) {
  if (!$content_type) {
    backdrop_set_message(t('Missing content type.'), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_sn');
  }

  $assignments = config_get('acuity_sn.title_assignment', 'assignments');
  if (empty($assignments[$content_type])) {
    backdrop_set_message(t('No title assignment found for %type.', ['%type' => $content_type]), 'error');
    backdrop_goto('admin/config/acuity-utils/acuity_sn');
  }

  $form['content_type'] = [
    '#type' => 'value',
    '#value' => $content_type,
  ];

  $form['message'] = [
    '#markup' => '<p>' . t('Are you sure you want to remove the title assignment for the content type <strong>@type</strong>?', ['@type' => node_type_get_name($content_type)]) . '</p>',
  ];

  $form['actions'] = [
    '#type' => 'actions',
    'submit' => [
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#submit' => ['acuity_sn_remove_title_assignment_form_submit'],
      '#button_type' => 'submit',
    ],
    'cancel' => [
      '#markup' => l(t('Cancel'), 'admin/config/acuity-utils/acuity_sn', ['attributes' => ['class' => ['button']]]),
    ],
  ];

  return $form;
}

/**
 * Submit handler for title assignment removal.
 */
function acuity_sn_remove_title_assignment_form_submit($form, &$form_state) {
  $content_type = $form_state['values']['content_type'];
  $config = config('acuity_sn.title_assignment');
  $assignments = $config->get('assignments') ?: [];

  unset($assignments[$content_type]);
  $config->set('assignments', $assignments)->save();

  backdrop_set_message(t('Title assignment for %type has been removed.', ['%type' => node_type_get_name($content_type)]));
  $form_state['redirect'] = 'admin/config/acuity-utils/acuity_sn';
}

/**
 * Help page callback.
 */
function acuity_sn_help_page() {
  $output = '';

  $output .= '<div class="acuity-sn-admin-section">';
  $output .= '<h3>' . t('About') . '</h3>';
  $output .= '<p>' . t('The Acuity Serial Number module provides a flexible system for generating atomic, auto-incrementing serial numbers. These numbers can be applied to <strong>Text Fields</strong> or to the <strong>Title</strong> of a content type.') . '</p>';
  $output .= '</div>';

  $output .= '<div class="acuity-sn-admin-section">';
  $output .= '<h3>' . t('Configuration') . '</h3>';
  $output .= '<p>' . t('Visit the admin page at !url', ['!url' => l('admin/config/acuity-utils/acuity_sn', 'admin/config/acuity-utils/acuity_sn')]) . '</p>';
  $output .= '<p>' . t('Set options for:') . '</p>';
  $output .= '<ul>';
  $output .= '<li><strong>' . t('Machine name') . ':</strong> ' . t('A machine readable field name that must consist of lowercase letters, numbers and underscores but it must start with a character. i.e. "invoice_number", "po_number", "widget_serial".') . '</li>';
  $output .= '<li><strong>' . t('Label') . ':</strong> ' . t('This is the human readable text and field label used on forms and when selecting the widget from the Content Type field settings.') . '</li>';
  $output .= '<li><strong>' . t('Prefix') . ':</strong> ' . t('Automatically added to the beginning of the serial number i.e. "Inv00125".') . '</li>';
  $output .= '<li><strong>' . t('Suffix') . ':</strong> ' . t('Automatically appended to the end of the serial number i.e. "Inv00125pending".') . '</li>';
  $output .= '<li><strong>' . t('Padding') . ':</strong> ' . t('Zero padding adds a 0 to the left of the serial number i.e. 4 = 0001, 5 = 00001.') . '</li>';
  $output .= '<li><strong>' . t('Next') . ':</strong> ' . t('Next serial number to use. This is also used for the starting number. This should be numeric with no padding i.e. "1".') . '</li>';
  $output .= '</ul>';

  $output .= '<h3>' . t('Usage') . '</h3>';
  $output .= '<p>' . t('<strong>How it works:</strong> The serial number field shows the next available serial number on the data entry screen as a <strong>read-only</strong> field. This serial number is just an indication of the next one available when the page was loaded; the system uses the next available atomic number when the content is actually saved to prevent duplicates.') . '</p>';
  $output .= '<ul>';
  $output .= '<li>' . t('<strong>Text Fields:</strong> Add/modify a <em>Text (short)</em> field widget to <strong>Auto serial number</strong>.') . '</li>';
  $output .= '<li>' . t('From the "Serial number group" selection, choose one of the "Serial number groups" created from the settings page.') . '</li>';
  $output .= '<li>' . t('The Text Processor should be "Plain text".') . '</li>';
  $output .= '<li>' . t('<strong>Node Titles:</strong> On the settings page, click "Assign serial number to title". Select the content type and the serial group.') . '</li>';
  $output .= '</ul>';

  $output .= '<h4>' . t('Important Behaviors') . '</h4>';
  $output .= '<ul>';
  $output .= '<li><strong>' . t('Permanent Assignment') . ':</strong> ' . t('Serial numbers are designed to be unique and strictly sequential (e.g., for Invoices, Purchase Orders, Batch Numbers). Once a number is assigned to a node, it is permanent. Deleting a node will <strong>not</strong> release that serial number for reuse; the counter will continue from the "Next number" defined in the settings.') . '</li>';
  $output .= '<li><strong>' . t('Settings are not Retroactive') . ':</strong> ' . t('Changing the settings for a Serial Group (e.g., altering the Prefix or Suffix) will <strong>not</strong> affect existing content. The serial number is saved as plain text in the database. The module only inserts a serial number if the field is empty; if a field already contains data, it will remain unchanged.') . '</li>';
  $output .= '</ul>';

  $output .= '<h4>' . t('Existing Content') . '</h4>';
  $output .= '<p>' . t('This module assigns numbers to new content going forward. It does not retroactively generate numbers for existing nodes.') . '</p>';
  $output .= '<p><strong>' . t('Important:') . '</strong> ' . t('If you are upgrading an existing content type where you previously manually entered serial numbers, changing the widget to "Auto serial number" will make those fields read-only. You will not be able to edit the existing values.') . '</p>';
  $output .= '<p><strong>' . t('Recommendation:') . '</strong> ' . t('Configure the Serial Number Group\'s "Next number" to be greater than your highest existing manual serial number to avoid conflicts.') . '</p>';
  $output .= '</div>';

  $output .= '<div class="acuity-sn-admin-section">';
  $output .= '<h3>' . t('Developer API') . '</h3>';
  $output .= '<p><strong>' . t('NOTE: The API is a work-in-progress... Please contact the developer before using the API in production.') . '</strong></p>';
  $output .= '<p>' . t('You can request a serial number programmatically using the following function:') . '</p>';
  $output .= '<pre><code>$serial = acuity_sn_generate_number(\'repair\');</code></pre>';
  $output .= '</div>';

  return $output;
}