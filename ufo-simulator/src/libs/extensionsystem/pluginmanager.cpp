#include "pluginmanager.h"
#include "pluginspec.h"
#include <QList>
//#include <global_const.h>
#include <QtCore>
#include <QSettings>
#include <QPluginLoader>
#include "iplugin.h"

using namespace ExtensionSystem;


PluginManager *PluginManager::m_instance = 0;

PluginManager::PluginManager()
{
    m_instance = this;
    //QDir dir(Global::Const::PluginPath);
    //this->LoadPluginSpec(dir);
    //this->connect(QCoreApplication::instance(),SIGNAL(aboutToQuit()),this,SLOT(aboutToClose()));
}


PluginManager* PluginManager::instance()
{
    return m_instance;
}


PluginManager::~PluginManager()
{
    for (QList<QObject* >::iterator it = this->m_allObjects.begin();
            it != this->m_allObjects.end();++it) {
        delete *it;
    }
}

void PluginManager::addObject(QObject *obj)
{
    this->m_allObjects.push_back(obj);
}

void PluginManager::removeObject(QObject *obj)
{
}

void PluginManager::loadPlugins()
{
#if 0
    QList<Inner::AuxLoadStruct> specList;
    QList<IPlugin* > olist;
    foreach(const PluginSpec& spec, this->m_pluginSpecs) {
        specList.push_back(Inner::AuxLoadStruct(spec));
    }

    for (;;) {
        bool conti = false;
        foreach(const Inner::AuxLoadStruct& spec, specList) {
            if (spec.depends.size() == 0) {
/// Load
                conti = true;
                QString fn("plugin/%1.%2");

/// Platform independence
#ifdef Q_OS_WIN32
                fn = fn.arg(spec.key.Name).arg("dll");
#endif
/// end
                qDebug()<<"Loading "<<fn;
                QPluginLoader loader(fn);
                bool loadSuccess = loader.load();

                if (!loadSuccess) {
                    qWarning() << tr("%1 Plugin Load Error ").arg(fn);
                }

                else {
                    QObject* obj = loader.instance();
                    this->addObject(obj);

                    IPlugin* plugin = qobject_cast<IPlugin*> (obj);
                    foreach(const PluginSpec& s, this->m_pluginSpecs) {
                        if (s.name() == spec.key.Name) {
                            plugin->setSpec(&s);
                            break;
                        }
                    }

                    olist.append(plugin);
                    plugin->Initialize(QCoreApplication::arguments());
                }

/// Set Statue

                for (QList<PluginSpec>::iterator it = this->m_pluginSpecs.begin();
                        it != this->m_pluginSpecs.end();++it) {
                    it->setState(PluginSpec::BeforeInit | PluginSpec::Loaded);
                }

/// Sovole Depend

                for (QList<Inner::AuxLoadStruct>::iterator it = specList.begin()
                        ;it != specList.end();++it) {
                    for (QList<SpecDependencyData>::iterator dit = it->depends.begin();
                            dit != it->depends.end();++dit) {
                        if (dit->Name == spec.key.Name
                                && dit->Version == spec.key.Version) {
                            it->depends.erase(dit);
                            break;
                        }
                    }
                }

                specList.removeOne(spec);
            }
        }

        if (!conti)
            break;
    }

    for (QList<IPlugin* >::iterator it = olist.begin();it != olist.end();++it) {
        IPlugin* plugin = *it;
        plugin->Initialized();

        int index = this->m_pluginSpecs.indexOf(*plugin->getSpec());

        this->m_pluginSpecs[index].setState(PluginSpec::Loaded | PluginSpec::Inited);
    }
#endif   
}

void ExtensionSystem::PluginManager::aboutToClose()
{
    foreach (QObject* obj, this->m_allObjects) {
        IPlugin* plugin = qobject_cast<IPlugin* >(obj);
        if(plugin!=0)
        {
            plugin->aboutToShutdown();
        }
    }
}

void PluginManager::LoadPluginSpec(const QDir &dir)
{
    QFileInfoList flist = dir.entryInfoList();

    foreach(const QFileInfo& inf, flist) {
        if (inf.isDir()
                && !inf.isSymLink()
                && inf.fileName() != "."
                && inf.fileName() != "..") {
            this->LoadPluginSpec(inf.dir());
        }

        else
            if (inf.completeSuffix() == "pluginspec") {
                m_pluginSpecs.append(PluginSpec(inf.filePath()));
            }
    }
}


namespace Inner {

struct AuxLoadStruct {
    SpecDependencyData key;
    QList<SpecDependencyData> depends;
    AuxLoadStruct(const PluginSpec& spec) {
        key.Name = spec.name();
        key.Version = spec.version();
        foreach(const SpecDependencyData& dep , spec.dependencyList()) {
            depends.push_back(dep);
        }
    }

    bool operator == (const AuxLoadStruct& other) {
        return this->key.Name == other.key.Name && this->key.Version == other.key.Version;
    }
};
}


