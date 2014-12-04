<?php

/**
 * @file
 *   Example drush command.
 *
 *   To run this *fun* command, execute `sudo drush --include=./examples mmas`
 *   from within your drush directory.
 *
 *   See `drush topic docs-commands` for more information about command authoring.
 *
 *   You can copy this file to any of the following
 *     1. A .drush folder in your HOME folder.
 *     2. Anywhere in a folder tree below an active module on your site.
 *     3. /usr/share/drush/commands (configurable)
 *     4. In an arbitrary folder specified with the --include option.
 *     5. Drupal's sites/all/drush folder.
 */

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function whiteboard_migration_drush_command() {
  $items = array();

  $items['migrate-whiteboard-invoices-generate'] = array(
    'description' => "Merges the header, footer & content for the invoices into a single html file.",
    'arguments' => array(),
    'options' => array(
      'invoice_nid' => array(
        'description' => 'Node id of the invoice we need to migrate.',
        'example-value' => '1252920',
      ),
      'invoice_vid' => array(
        'description' => 'Revision id of the invoice we need to migrate.',
        'example-value' => '1252920',
      ),
      'limit' => array(
        'description' => 'Generate HTML invoice for first n invoices that pass the criteria',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwi --invoice_id=1252920' => 'Generate markup for invoice from database records.',
    ),
    'aliases' => array('mwig'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-invoices-upload'] = array(
    'description' => "Uploads HTML invoices to amazon s3 bucket & saves the linkback in db",
    'arguments' => array(),
    'examples' => array(
      'drush mwiu' => 'Move the markup for invoice id 1252920 to amazon s3 bucket.',
    ),
    'aliases' => array('mwiu'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-payments-generate'] = array(
    'description' => "Generates HTML file for the markup stored in db.",
    'arguments' => array(),
    'options' => array(
      'limit' => array(
        'description' => 'Generate HTML invoice for first n invoices that pass the criteria',
        'example-value' => '10',
      ),
    ),
    'examples' => array(
      'drush mwpg --limit=100' => 'Generate markup for invoice from database records.',
    ),
    'aliases' => array('mwpg'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  $items['migrate-whiteboard-payments-upload'] = array(
    'description' => "Uploads HTML payments to amazon s3 bucket & saves the linkback in db",
    'arguments' => array(),
    'examples' => array(
      'drush mwiu' => 'Move the markup for payment id 1252920 to amazon s3 bucket.',
    ),
    'aliases' => array('mwpu'),
    'bootstrap' => DRUSH_BOOTSTRAP_FULL, // No bootstrap at all.
  );

  return $items;
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_migrate_whiteboard_invoices_generate() {
  if (($invoice_nid = drush_get_option('invoice_nid')) && ($invoice_vid = drush_get_option('invoice_vid'))) {
    drush_print('Migrating invoice nid: ' . drush_get_option('invoice_nid') . ' & vid: ' . drush_get_option('invoice_vid'));
    whiteboard_migration_prepare_html_invoice($invoice_nid, $invoice_vid);
  }
  else {
    $invoices = whiteboard_migration_select_invoices(drush_get_option('limit'));
    drush_print('Migrating ' . count($invoices) . ' invoices.');
    whiteboard_migration_create_batch($invoices, 50,
      'whiteboard_migration_batch_process_invoices_create',
      'whiteboard_migration_invoice_batch_complete',
      'Batch Process for generating HTML invoices'
    );
  }
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_migrate_whiteboard_invoices_upload() {
  $files = whiteboard_migration_select_unmigrated_files('invoice');
  whiteboard_migration_create_batch($files, 50,
    'whiteboard_migration_move_invoice_files_to_s3',
    'whiteboard_migration_invoice_batch_complete',
    'Batch Process for moving files to s3.'
  );
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_migrate_whiteboard_payments_upload() {
  $files = whiteboard_migration_select_unmigrated_files('payment');
  whiteboard_migration_create_batch($files, 50,
    'whiteboard_migration_move_invoice_files_to_s3',
    'whiteboard_migration_invoice_batch_complete',
    'Batch Process for moving files to s3.'
  );
}

/**
 * drush command callback. This is where the action takes place.
 */
function drush_whiteboard_migration_migrate_whiteboard_payments_generate() {
  $payments = whiteboard_migration_fetch_payments();

  whiteboard_migration_create_batch($payments, 50,
    'whiteboard_migration_prepare_markup',
    'whiteboard_migration_invoice_batch_complete',
    'Batch Process for moving files to s3.'
  );
}
