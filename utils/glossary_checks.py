#!/usr/bin/env python
# encoding: utf-8

"""Runs a glossary check on all po files and writes the check results to
po_validation/glossary.

You will need to have the Translate Toolkit installed in order for the checks to work:
http://toolkit.translatehouse.org/

You will need to provide an export of the Transifex glossary and specify it at
the command line. Make sure to select "Include glossary notes in file" when
exporting the csv from Transifex.

Translators can 'misuse' their languages' comment field on Transifex to add
inflected forms of their glossary translations. We use the delimiter '|' to
signal that the field has inflected forms in it. Examples:

Source    Translation    Comment             Translation will be matched against
------    -----------    ----------------    -----------------------------------
sheep     sheep          Nice, fluffy!       'sheep'
ax        axe            axes|               'axe', 'axes'
click     click          clicking|clicked    'click', 'clicking', 'clicked'

"""

from collections import defaultdict
from subprocess import call
import csv
import os.path
import re
import sys
import traceback


def read_csv_file(filepath):
    """Parses a CSV file into a 2-dimensional array."""
    result = []
    with open(filepath) as csvfile:
        csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
        for row in csvreader:
            result.append(row)
    return result


def is_vowel(character):
    """Helper function for creating inflections of English words."""
    return character == 'a' or character == 'e' or character == 'i' \
        or character == 'o' or character == 'u' or character == 'y'


def make_english_plural(word):
    """Create plural forms for nouns.

    This will create a few nonsense entries for irregular plurals, but
    it's good enough for our purpose. Glossary contains pluralized
    terms, so we don't add any plural forms for strings ending in 's'.

    """
    result = ''
    if not word.endswith('s'):
        if word.endswith('y') and not is_vowel(word[-2:-1]):
            result = word[0:-1] + 'ies'
        elif word.endswith('z') or word.endswith('x'):
            result = word + 'es'
        else:
            result = word + 's'
    return result


def make_english_verb_forms(word):
    """Create inflected forms of an English verb: -ed and -ing forms.

    Will create nonsense for irregular verbs.

    """
    result = []
    if word.endswith('e'):
        result.append(word[0:-1] + 'ing')
        result.append(word + 'd')
    elif is_vowel(word[-2:-1]) and not is_vowel(word[-1]):
        # The consonant is duplicated here if the last syllable is stressed.
        # We can't detect stress, so we add both variants.
        result.append(word + word[-1] + 'ing')
        result.append(word + 'ing')
        result.append(word + word[-1] + 'ed')
        result.append(word + 'ed')
    elif word.endswith('y') and not is_vowel(word[-2:-1]):
        result.append(word + 'ing')
        result.append(word[0:-1] + 'ed')
    else:
        result.append(word + 'ing')
        result.append(word + 'ed')
    return result


class glossary_entry:
    """An entry in our parsed glossaries."""

    def __init__(self):
         # Base form of the term, followed by any inflected forms
        self.terms = []
        # Base form of the translation, followed by any inflected forms
        self.translations = []


def load_glossary(glossary_file, locale):
    """Build a glossary from the given Transifex glossary csv file for the
    given locale."""
    result = []
    counter = 0
    term_index = 0
    wordclass_index = 0
    translation_index = 0
    comment_index = 0
    for row in read_csv_file(glossary_file):
        # Detect the column indices
        if counter == 0:
            colum_counter = 0
            for header in row:
                if header == 'term':
                    term_index = colum_counter
                elif header == 'pos':
                    wordclass_index = colum_counter
                elif header == 'translation_' + locale:
                    translation_index = colum_counter
                elif header == 'comment_' + locale:
                    comment_index = colum_counter
                colum_counter = colum_counter + 1
        # If there is a translation, parse the entry
        elif len(row[translation_index].strip()) > 0:
            entry = glossary_entry()
            entry.terms.append(row[term_index].strip())
            if row[wordclass_index] == 'Noun':
                plural = make_english_plural(entry.terms[0])
                if len(plural) > 0:
                    entry.terms.append(plural)
            elif row[wordclass_index] == 'Verb':
                verb_forms = make_english_verb_forms(entry.terms[0])
                for verb_form in verb_forms:
                    entry.terms.append(verb_form)

            entry.translations.append(row[translation_index].strip())

            # Misuse the comment field to provide a list of inflected forms.
            # Otherwise, we would get tons of false positive hits in the checks
            # later on and the translators would have our heads on a platter.
            delimiter = '|'
            if len(row[comment_index].strip()) > 1 and delimiter in row[comment_index]:
                inflections = row[comment_index].split(delimiter)
                for inflection in inflections:
                    entry.translations.append(inflection.strip())

            result.append(entry)
        counter = counter + 1
    return result


class failed_entry:
    """Information about a translation that failed a check."""

    def __init__(self):
        # The locale where the check failed
        self.locale = ''
        # The po file containing the failed translation
        self.po_file = ''
        # Source text
        self.source = ''
        # Target text
        self.target = ''
        # Location in the source code
        self.location = ''
        # The glossary term that failed the check
        self.term = ''
        # The base form of the translated glossary term
        self.translation = ''


def contains_term(string, term):
    """Checks whether 'string' contains 'term' as a whole word.

    This check is case-ionsensitive.

    """
    result = False
    # Regex is slow, so we do this preliminary check
    if term.lower() in string.lower():
        # Now make sure that it's whole words!
        # We won't want to match "AI" against "again" etc.
        regex = re.compile('^|(.+\W)' + term + '(\W.+)|$', re.IGNORECASE)
        result = regex.match(string)
    return result


def source_contains_term(source_to_check, entry, term, glossary):
    """Check if the source string contains the glossary term while filtering
    out superstrings from the glossary, e.g. we don't want to check 'arena'
    against 'battle arena'."""
    result = False
    source_to_check = source_to_check.lower()
    term = term.lower()
    if term in source_to_check:
        source_regex = re.compile('.+[\s,.]' + term + '[\s,.].+')
        if source_regex.match(source_to_check):
            for entry2 in glossary:
                if entry.terms[0] != entry2.terms[0]:
                    for term2 in entry2.terms:
                        term2 = term2.lower()
                        if term2 != term and term in term2 and term2 in source_to_check:
                            source_to_check = source_to_check.replace(
                                term2, '')
            # Check if the source still contains the term to check
            result = contains_term(source_to_check, term)
    return result


def check_file(csv_file, glossaries, locale, po_file):
    """Run the actual check."""
    translations = read_csv_file(csv_file)
    source_index = 0
    target_index = 0
    location_index = 0
    hits = []
    counter = 0
    for row in translations:
        # Detect the column indices
        if counter == 0:
            colum_counter = 0
            for header in row:
                if header == 'source':
                    source_index = colum_counter
                elif header == 'target':
                    target_index = colum_counter
                elif header == 'location':
                    location_index = colum_counter
                colum_counter = colum_counter + 1
        else:
            for entry in glossaries[locale][0]:
                for term in entry.terms:
                    # Check if the source text contains the glossary term.
                    # Filter out superstrings, e.g. we don't want to check
                    # "arena" against "battle arena"
                    if source_contains_term(row[source_index], entry, term, glossaries[locale][0]):
                        # Now verify the translation against all translation
                        # variations from the glossary
                        translation_has_term = False
                        for translation in entry.translations:
                            if contains_term(row[target_index], translation):
                                translation_has_term = True
                                break
                        if not translation_has_term:
                            hit = failed_entry()
                            hit.source = row[source_index]
                            hit.target = row[target_index]
                            hit.location = row[location_index]
                            hit.term = entry.terms[0]
                            hit.translation = entry.translations[0]
                            hit.locale = locale
                            hit.po_file = po_file
                            hits.append(hit)
        counter = counter + 1
    return hits


def make_path(base_path, subdir):
    """Creates the correct form of the path and makes sure that it exists."""
    result = os.path.abspath(os.path.join(base_path, subdir))
    if not os.path.exists(result):
        os.makedirs(result)
    return result


def delete_path(path):
    """Deletes the directory specified by 'path' and all its subdirectories and
    file contents."""
    if os.path.exists(path) and not os.path.isfile(path):
        files = sorted(os.listdir(path), key=str.lower)
        for deletefile in files:
            deleteme = os.path.abspath(os.path.join(path, deletefile))
            if os.path.isfile(deleteme):
                try:
                    os.remove(deleteme)
                except Exception:
                    print('Failed to delete file ' + deleteme)
            else:
                delete_path(deleteme)
        try:
            os.rmdir(path)
        except Exception:
            print('Failed to delete path ' + deleteme)


def check_translations_with_glossary(input_path, output_path, glossary_file):
    """Main loop.

    Loads the Transifex glossary, converts all po files for languages
    that have glossary entries to csv files, runs the check and then
    writes any hits into csv files.

    """
    csv_path = make_path(output_path, 'csv')
    hits = []
    locale_list = []

    glossaries = defaultdict(list)
    source_directories = sorted(os.listdir(input_path), key=str.lower)
    for dirname in source_directories:
        dirpath = os.path.join(input_path, dirname)
        if os.path.isdir(dirpath):
            source_files = sorted(os.listdir(dirpath), key=str.lower)
            print('Checking ' + dirname)
            failed = 0
            for source_filename in source_files:
                po_file = dirpath + '/' + source_filename
                if source_filename.endswith('.po'):
                    locale = source_filename[0:-3]
                    # Load the glossary if we haven't seen this locale before
                    if len(glossaries[locale]) < 1:
                        sys.stdout.write('Loading glossary for ' + locale)
                        glossaries[locale].append(
                            load_glossary(glossary_file, locale))
                        sys.stdout.write(' - %d entries\n' %
                                         len(glossaries[locale][0]))
                        sys.stdout.flush()
                    # Only bother with locales that have glossary entries
                    if len(glossaries[locale][0]) > 0:
                        locale_list.append(locale)
                        csv_file = os.path.abspath(os.path.join(
                            csv_path, dirname + '_' + locale + '.csv'))
                        # Convert to csv for easy parsing
                        call(['po2csv', '--progress=none', po_file, csv_file])

                        # Now run the actual check
                        current_hits = check_file(
                            csv_file, glossaries, locale, dirname)
                        for hit in current_hits:
                            hits.append(hit)

                        # The csv file is no longer needed, delete it.
                        os.remove(csv_file)

    for locale in locale_list:
        locale_result = '"glossary_term","glossary_translation","source","target","file","location"\n'
        for hit in hits:
            if hit.locale == locale:
                row = '"%s","%s","%s","%s","%s","%s"\n' % (
                    hit.term, hit.translation, hit.source, hit.target, hit.po_file, hit.location)
                locale_result = locale_result + row
        dest_filepath = output_path + '/glossary_check_' + locale + '.csv'
        with open(dest_filepath, 'wt') as dest_file:
            dest_file.write(locale_result)

    delete_path(csv_path)
    return 0


def main():
    """Checks whether we are in the correct directory and everything's there,
    then runs a glossary check over all PO files."""
    if (len(sys.argv) == 2):
        print('Running glossary checks:')
    else:
        print('Usage: utils/glossary_checks.py <relative-path-to-glossary>')
        return 1

    try:
        # Prepare the paths
        glossary_file = os.path.abspath(os.path.join(
            os.path.dirname(__file__), sys.argv[1]))

        if (not (os.path.exists(glossary_file) and os.path.isfile(glossary_file))):
            print('There is no glossary file at ' + glossary_file)
            return 1

        input_path = os.path.abspath(os.path.join(
            os.path.dirname(__file__), '../po'))
        output_path = make_path(os.path.dirname(__file__), '../po_validation')
        return check_translations_with_glossary(input_path, output_path, glossary_file)

    except Exception:
        print('Something went wrong:')
        traceback.print_exc()
        delete_path(make_path(output_path, 'csv'))
        return 1

if __name__ == '__main__':
    sys.exit(main())
