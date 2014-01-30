#!/usr/bin/env python
# -*- coding: utf-8 -*-

from scletest.sclehooq import ApplicationRegistry
from nose.plugins import Plugin
from ConfigParser import ConfigParser
import os
import logging

LOG = logging.getLogger('nose.plugins.scle')

def log_with_sep(message):
    sep = '-' * 70
    LOG.info(sep)
    LOG.info(message)
    LOG.info(sep)

# création d'un Application registry global
_APP_REGISTRY = ApplicationRegistry()
config = _APP_REGISTRY.config

class SclePlugin(Plugin):
    name = 'scle'
    
    def options(self, parser, env=os.environ):
        super(SclePlugin, self).options(parser, env=env)
        parser.add_option('--scle-conf',
                          dest='scle_conf',
                          default=env.get('NOSE_SCLE_CONF') or 'scletest.conf')
    
    def configure(self, options, conf):
        Plugin.configure(self, options, config)
        if not self.enabled:
            return
        conf_file = os.path.realpath(options.scle_conf)
        if not os.path.isfile(conf_file):
            raise Exception("Fichier de conf scle manquant: `%s`" % conf_file)
        conf = ConfigParser()
        conf.read([conf_file])
        _APP_REGISTRY.register_from_conf(conf, os.path.dirname(conf_file))

    def beforeTest(self, test):
        log_with_sep(u"Démarrage de test `%s`" % unicode(test.id(), 'utf-8'))
    
    def afterTest(self, test):
        log_with_sep(u"Fin de test `%s`" % unicode(test.id(), 'utf-8'))
