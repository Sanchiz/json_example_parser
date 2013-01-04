<?php
/**
 * @file
 * Contains FeedsSitemapParser and related classes.
 */

/**
 * A JSON example parser
 */
class JsonExampleParser extends FeedsParser {
  /**
   * Implements FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    $result = new FeedsParserResult();
    // получаем последние настройки
    $source_config = $source->getConfigFor($this);
    // извлекаем JSON данные
    $fetch_items = json_decode($fetcher_result->getRaw());
    foreach ($fetch_items as $value) {
      $item = array('name' => $value->name);
      if ($value->year) {
        $item['year'] = intval($value->year);
      }
      if ($value->price) {
        $item['price'] = floatval($value->price);
      }
      // применение настроек из формы
      if (  $source_config['type'] == 'all' ||
            ($source_config['type'] == 'free' && $item['price'] == 0) ||
            ($source_config['type'] == 'paid' && $item['price'] > 0)) {
        // добавляем запись для импорта
        $result->items[] = $item;
      }
    }
    return $result;
  }

  /**
   * Implements FeedsParser::getMappingSources().
   */
  public function getMappingSources() {
    return array(
      'name' => array(
        'name' => t('Game name'),
        'description' => t('The name of the computer game.'),
      ),
      'year' => array(
        'name' => t('Release year'),
        'description' => t('Release year of the computer game.'),
      ),
      'price' => array(
        'name' => t('Game price'),
        'description' => t('The cost of the computer game.'),
      ),
    ) + parent::getMappingSources();
  }

  /**
   * Configuration form.
   * Конфигурационная форма, которая будет отображаться на сранице настройки импортера.
   */
  public function configForm(&$form_state) {
    $form = array();
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Game type'),
      '#description' => t('Game filter by type.'),
      '#options' => array(
        'all' => t('All game'),
        'paid' => t('Paid'),
        'free' => t('Free'),
      ),
      '#default_value' => $this->config['type'],
    );
    return $form;
  }

  /**
   * Define default configuration values.
   * Стандартные настройки для парсера, которые будут применены если форма открыта впервые.
   */
  public function configDefaults() {
    return array(
      'type' => 'all',
    );
  }

  /**
   * Define defaults.
   * Определение настроек отправленных из формы.
   */
  public function sourceDefaults() {
    return array(
      'type' => $this->config['type'],
    );
  }

  /**
   * Show configuration form for users.
   * Конфигурационная форма которая будет отображаться пользователям на странице импорта.
   */
  public function sourceForm($source_config) {
    $form = array();
    $form['#weight'] = -10;
    $form['help']['#markup'] = '<div class="help"><p>' . t('Select the type of game you want to import') . ':</p></div>';
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Game type'),
      '#description' => t('Game filter by type.'),
      '#options' => array(
        'all' => t('All game'),
        'paid' => t('Paid'),
        'free' => t('Free'),
      ),
      '#default_value' => isset($source_config['type']) ? $source_config['type'] : 'all',
    );
    return $form;
  }
}
