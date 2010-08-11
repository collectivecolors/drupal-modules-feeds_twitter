<?php
// $Id$
/**
 * @file
 *  Feeds parser class for Twitter.

/**
 * Class definition for Twitter Atom Parser.
 *
 * Parses Atom Twitter feeds.
 */
class FeedsTwitterAtomParser extends FeedsSimplePieParser {
  
  // YouTube feed namespaces.
  
  const NAMESPACE_GOOGLE = 'http://base.google.com/ns/1.0';
  const NAMESPACE_TWITTER = 'http://api.twitter.com/';
  
  /**
   * Parse the extra mapping sources provided by this parser.
   */
  protected function parseExtensions(&$item, $simplepie_item) {
    
    unset($item['author_email']); // YouTube does not supply this information.
    unset($item['tags']); // Tweets are not categorized.  
    unset($item['domains']); // TODO:  Consider using Open Calais to tag Tweets?
    
    // Add extra Twitter fields.
    
    $item['author_profile_image'] = $simplepie_item->get_link(0, 'image');
        
    if ($lang = $simplepie_item->get_item_tags(self::NAMESPACE_TWITTER, 'lang')) {
      $item['language'] = $lang[0]['data'];
    }
        
    if ($location = $simplepie_item->get_item_tags(self::NAMESPACE_GOOGLE, 'location')) {
      $item['location'] = $location[0]['data'];
    }
  }

  /**
   * Add the extra mapping sources provided by this parser.
   */
  public function getMappingSources() {
    $map = parent::getMappingSources();
    
    // Not used.
    unset($map['author_email']);
    unset($map['tags']);
    unset($map['domains']);
    unset($map['location_latitude']);
    unset($map['location_longitude']);
    unset($map['enclosures']);
    
    $map['description']['name'] = t('Full post');
    
    // Add our fields.
    $mapping = $map + array(      
      'author_profile_image' => array(
        'name' => t('Author profile image'),
        'description' => t('Link to author profile thumbnail.'),
      ),
      'language' => array(
        'name' => t('Language'),
        'description' => t('Language code of the post.'),
      ),
      'location' => array(
        'name' => t('Location'),
        'description' => t('Text based representation of the post or poster\'s location.'),
      ),      
    );
    
    // Sort them.
    asort($mapping);
    return $mapping;
  }
}
