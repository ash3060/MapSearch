# -*- coding: utf-8 -*-
"""
Django settings for db_project project.

For more information on this file, see
https://docs.djangoproject.com/en/1.6/topics/settings/

For the full list of settings and their values, see
https://docs.djangoproject.com/en/1.6/ref/settings/
"""

# Build paths inside the project like this: os.path.join(BASE_DIR, ...)
import os
import json
BASE_DIR = os.path.dirname(os.path.dirname(__file__))


# Quick-start development settings - unsuitable for production
# See https://docs.djangoproject.com/en/1.6/howto/deployment/checklist/

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = 'usx96@or4r$mxrldorb1jo3w5+m5ikuod#qdfr*&6z80+w^s6_'

# SECURITY WARNING: don't run with debug turned on in production!
DEBUG = True

TEMPLATE_DEBUG = True

ALLOWED_HOSTS = []


# Application definition

INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
)

MIDDLEWARE_CLASSES = (
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
)

ROOT_URLCONF = 'db_project.urls'

WSGI_APPLICATION = 'db_project.wsgi.application'


# Database
# https://docs.djangoproject.com/en/1.6/ref/settings/#databases

DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.sqlite3',
        'NAME': os.path.join(BASE_DIR, 'db.sqlite3'),
    }
}

# Internationalization
# https://docs.djangoproject.com/en/1.6/topics/i18n/

LANGUAGE_CODE = 'en-us'

TIME_ZONE = 'UTC'

USE_I18N = True

USE_L10N = True

USE_TZ = True


# Static files (CSS, JavaScript, Images)
# https://docs.djangoproject.com/en/1.6/howto/static-files/

STATIC_URL = '/static/'
TEMPLATE_DIRS = (
    os.path.join(BASE_DIR, "templates"),
)
STATICFILES_DIRS = (
    os.path.join(BASE_DIR, "static"),
)


class poi(object):
    def __init__(self, _lat, _lng, _tags):
        self.lat = _lat
        self.lng = _lng
        self.tags = _tags


poi_list = []
data_file = open('zipcode-address.json')
lines = data_file.readlines()
for line in lines:
    line = line.strip()
    entry = json.loads(line)
    lat = float(entry["latlng"][0])
    lng = float(entry["latlng"][1])
    tags = []
    for word in entry["name"].split(' '):
        if word == '':
            continue
        word = word.lower()
        tags = tags + [word]
    tmp_entry = poi(lat, lng, tags)
    poi_list = poi_list + [tmp_entry]


RECORD_LIST = poi_list
print 'INDEX_BUILDING FINISHED...'
