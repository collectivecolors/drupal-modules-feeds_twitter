<?php
// $Id$

/**
 * @file
 * Home of the FeedsYouTubeVideoSearchFetcher.
 */

/**
 * Fetches YouTube videos based on sepecified search criteria.
 *   
 * This class basically exists to make creating feed nodes that fetch YouTube 
 * videos easier for the end user.
 */
class FeedsTwitterSearchFetcher extends FeedsHTTPFetcher {
  
  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    $defaults = parent::configDefaults();
    unset($defaults['auto_detect_feeds']);
    
    $defaults['search_url'] = 'http://search.twitter.com/search.%format';
    $defaults['format']     = 'atom';
       
    return $defaults;
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = parent::configForm($form_state);
    unset($form['auto_detect_feeds']);
    
    $form['search_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Twitter feed endpoint'),
      '#description' => t('You may use <b>%format</b> as a placeholder for the feed format.  If you change this URL be sure that the same query parameters are used.  If you empty this input then submit the default URL will be used'),
      '#default_value' => $this->config['search_url'],
    );
    $form['format'] = array(
      '#type' => 'select',
      '#title' => t('Format'),
      '#options' => array(
        'atom' => t('Atom'),
        'json' => t('JSON'),
      ),
      '#default_value' => $this->config['format'],
    );
    
    return $form;
  }
  
  /**
   * Override parent::configFormValidate().
   */
  public function configFormValidate(&$values) {
    parent::configFormValidate($values);
    
    if (!$values['search_url']) { // TODO: This should really be a check for a valid URL.
      $defaults = $this->configDefaults();
      $values['search_url'] = $defaults['search_url'];
    }
  }
  
  /**
   * Expose source form.
   */
  public function sourceForm($source_config) {
    ctools_include('dependent');

    $form['manual_search'] = array(
      '#type' => 'checkbox',
      '#id' => 'twitter-manual-search',
      '#title' => t('Manually enter search query'),
      '#default_value' => ($source_config['manual_search'] ? $source_config['manual_search'] : 0),
    );
    
    // Manual search form.
    
    $form['query'] = array(
      '#type' => 'textfield',
      '#title' => t('Search query'),
      '#description' => t('You can find information on the available search operators in the !documentation', array('!documentation' => l(t('documentation'), 'http://search.twitter.com/operators', array('attributes' => array('target' => '_blank'))))),
      '#default_value' => ($source_config['query'] ? $source_config['query'] : ''),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(1)),
    );    
    
    // Guided search form.
    
    $form['keywords'] = array(
      '#type' => 'textfield',
      '#title' => t('Keywords'),
      '#description' => t('Enter one of more Twitter keywords separated by a comma,.'),
      '#default_value' => isset($source_config['keywords']) ? $source_config['keywords'] : '',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );
    $form['tag_mode'] = array(
      '#type' => 'select',
      '#title' => t('Tag mode'),
      '#options' => array(
        'ALL' => t('Match All'),
        'ANY' => t('Match Any'),
      ),
      '#default_value' => isset($source_config['tag_mode']) ? $source_config['tag_mode'] : 'ALL',
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );    
    $form['exclude'] = array(
      '#type' => 'textfield',
      '#title' => t('Exclude these words'),
      '#description' => t('Enter one of more keywords separated by a comma.'),
      '#default_value' => ($source_config['exclude'] ? $source_config['exclude'] : ''),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="clear: both;"></div>',
    );    
    $form['hashtags'] = array(
      '#type' => 'textfield',
      '#title' => t('Search subjects (hashtags)'),
      '#description' => t('Enter one of more hashtags (without the # prefix) separated by a comma.'),
      '#default_value' => ($source_config['hashtags'] ? $source_config['hashtags'] : ''),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
    );
    
    $form['from_user'] = array(
      '#type' => 'textfield',
      '#title' => t('From user'),
      '#description' => t('Enter a single Twitter user (without the @ prefix).'),
      '#default_value' => ($source_config['from_user'] ? $source_config['from_user'] : ''),
      '#size' => 50,
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );  
    $form['to_user'] = array(
      '#type' => 'textfield',
      '#title' => t('To user'),
      '#description' => t('Enter a single Twitter user (without the @ prefix).'),
      '#default_value' => ($source_config['to_user'] ? $source_config['to_user'] : ''),
      '#size' => 50,
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );
    $form['referenced_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Referenced user'),
      '#description' => t('Enter a single Twitter user (without the @ prefix).'),
      '#default_value' => ($source_config['referenced_user'] ? $source_config['referenced_user'] : ''),
      '#size' => 50,
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );    
        
    $form['positive_attitude'] = array(
      '#type' => 'checkbox',
      '#title' => t('Positive attitude'),
      '#default_value' => ($source_config['positive_attitude'] ? $source_config['positive_attitude'] : 0),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
      '#prefix' => '<div style="clear: both;"></div>',
    );    
    $form['negative_attitude'] = array(
      '#type' => 'checkbox',
      '#title' => t('Negative attitude'),
      '#default_value' => ($source_config['negative_attitude'] ? $source_config['negative_attitude'] : 0),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
    );    
    $form['question'] = array(
      '#type' => 'checkbox',
      '#title' => t('Question'),
      '#default_value' => ($source_config['question'] ? $source_config['question'] : 0),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
    );    
    $form['contains_links'] = array(
      '#type' => 'checkbox',
      '#title' => t('Contains links'),
      '#default_value' => ($source_config['contains_links'] ? $source_config['contains_links'] : 0),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-manual-search' => array(0)),
    );
    
    $form['use_location'] = array(
      '#type' => 'checkbox',
      '#id' => 'twitter-search-location',
      '#title' => t('Limit posts to a specific geographic area'),
      '#default_value' => ($source_config['use_location'] ? $source_config['use_location'] : 0),
      '#prefix' => '<div style="clear: both;"></div>',
    );
    $form['location'] = array(
      '#type' => 'textfield',
      '#title' => t('Geo-Coordinates for search query'),
      '#description' => t('Enter a latitude and longitude separated by a comma.'),
      '#default_value' => ($source_config['location'] ? $source_config['location'] : ''),
      //'#size' => 15,
      //'#maxlength' => 15,
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-search-location' => array(1)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );
    $form['distance'] = array(
      '#type' => 'textfield',
      '#title' => t('Distance from coordinates'),
      //'#description' => t('Distance (radius) from coordinates to include.'),
      '#default_value' => ($source_config['distance'] ? $source_config['distance'] : 1),
      '#size' => 15,
      '#maxlength' => 15,
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-search-location' => array(1)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );    
    $form['distance_unit'] = array(
      '#type' => 'select',
      '#title' => t('Unit'),
      '#options' => array(
        'mi' => t('Miles'),
        'km' => t('Kilometers'),        
      ),
      '#default_value' => ($source_config['distance_unit'] ? $source_config['distance_unit'] : 'mi'),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array('twitter-search-location' => array(1)),
      '#prefix' => '<div style="float: left;">',
      '#suffix' => '</div>',
    );
    
    $form['language'] = array(
      '#type' => 'select',
      '#title' => t('Language'),
      '#options' => array(
        'all' => t('Any language'),
        'ar' => t('Arabic'),
        'da' => t('Danish'),
        'nl' => t('Dutch'),
        'en' => t('English'),
        'fa' => t('Farsi / Persian'),
        'fi' => t('Finnish'),
        'fr' => t('French'),
        'de' => t('German'),
        'hu' => t('Hungarian'),
        'is' => t('Icelandic'),
        'it' => t('Italian'),
        'ja' => t('Japanese'),
        'no' => t('Norwegian'),
        'pl' => t('Polish'),
        'pt' => t('Portuguese'),
        'ru' => t('Russian'),
        'es' => t('Spanish'),
        'sv' => t('Swedish'),
        'th' => t('Thai'),  
      ),
      '#default_value' => ($source_config['language'] ? $source_config['language'] : 'en'),
      '#prefix' => '<div style="clear: both;"></div>',
    );

    $form['result_type'] = array(
      '#type' => 'select',
      '#title' => t('Result type'),
      '#options' => array(
        'recent' => t('Most Recent'),
        'popular' => t('Most Popular'),
        'mixed' => t('Recent and Popular'),
      ),
      '#default_value' => ($source_config['result_type'] ? $source_config['result_type'] : 'recent'),
    );
    
    return $form;
  }

  /**
   * Override parent::sourceFormValidate().
   */
  public function sourceFormValidate(&$values) {
    
    if ($values['use_location']) {
      list($latitude, $longitude) = explode(',', $values['location']);
      
      $latitude  = trim($latitude);
      $longitude = trim($longitude);
      $distance  = trim($values['distance']);
      
      if (!is_numeric($latitude) || !is_numeric($longitude)) {
        form_set_error('location', t('Location must be in the format; latitude, longitude.'));
        $error = TRUE;  
      }
      if (!is_numeric($distance) || $distance <= 0) {
        form_set_error('distance', t('Distance must be a numeric value greater than zero.'));
        $error = TRUE;  
      }
    }    
    if ($error) { return; }
    
    $params = array(
      'q' => '',
      'result_type' => $values['result_type'],
      'lang' => $values['language'],
    );
    
    if ($values['manual_search']) {
      // Get Twitter query.
      $params['q'] = trim($values['query']);  
    }
    else {
      // Get keyword query.
      if ($values['keywords']) {
        $keywords = array();
      
        // Clean up specified Twitter keywords before query injection.
        foreach (explode(',', $values['keywords']) as $keyword) {
          $keywords[] = $this->encodeWord($keyword);
        }
        $keyword_join  = ($values['tag_mode'] == 'ALL' ? ' ' : ' OR ');    
        $params['q']  .= implode($keyword_join, $keywords) . ' ';
      }
      
      // Get exclude query.
      if ($values['exclude']) {
        $excludes = array();
      
        // Clean up specified Twitter excluded keywords before query injection.
        foreach (explode(',', $values['exclude']) as $exclude) {
          $excludes[] = '-' . $this->encodeWord($exclude);
        }
        $params['q'] .= implode(' ', $excludes) . ' ';
      }
      
      // Get hashtags query.
      if ($values['hashtags']) {
        $hashtags = array();
      
        // Clean up specified Twitter hashtags before query injection.
        foreach (explode(',', $values['hashtags']) as $tag) {
          $hashtags[] = '#' . preg_replace('/\#/', '', $this->encodeWord($tag, FALSE));
        }
        $params['q'] .= implode(' ', $hashtags) . ' ';
      }
      
      // Get user queries.
      if ($values['from_user']) {
        $params['q'] .= 'from:' . preg_replace('/\@/', '', $this->encodeWord($values['from_user'], FALSE)) . ' ';  
      }
      if ($values['to_user']) {
        $params['q'] .= 'to:' . preg_replace('/\@/', '', $this->encodeWord($values['to_user'], FALSE)) . ' ';  
      }
      if ($values['referenced_user']) {
        $params['q'] .= '@' . preg_replace('/\@/', '', $this->encodeWord($values['referenced_user'], FALSE)) . ' ';  
      }
      
      // Get attitude queries.
      if ($values['positive_attitude']) {
        $params['q'] .= ':) ';  
      }
      if ($values['negative_attitude']) {
        $params['q'] .= ':( ';  
      }
      if ($values['question']) {
        $params['q'] .= '? ';  
      }
      
      // Get links query.
      if ($values['contains_links']) {
        $params['q'] .= 'filter:links ';  
      }
    }
    
    // Wipe trailing space.
    $params['q'] = trim($params['q']);  
    
    // Query location radius.
    if ($values['use_location']) {
      $params['geocode'] = "$latitude,$longitude,$distance". $values['distance_unit'];
    }
    
    // Inject format.
    $feed_url = strtr($this->config['search_url'], array('%format' => $this->config['format']));
    
    // Form HTTP fetcher link.
    $query = drupal_query_string_encode($params);
    $values['source'] = "$feed_url?$query";  
  }
  
  /*
   * Clean and encode a single word for the Twitter query.
   */
  protected function encodeWord($word, $ensure_quote = TRUE) {
    $base = trim($word);
    
    if ($ensure_quote) {
      return '"'. preg_replace(array('/^\"/', '/\"$/'), '', $base) . '"';
    }
    return $base;  
  }
}