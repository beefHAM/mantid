﻿#include "MantidMDAlgorithms/OriginParameter.h"

namespace Mantid
{
    namespace MDAlgorithms
    {

        OriginParameter::OriginParameter(double n1, double n2, double n3) 
        {
            m_origin.push_back(n1);
            m_origin.push_back(n2);
            m_origin.push_back(n3);
            m_isValid = true;
        }

        OriginParameter::OriginParameter()
        {
            this->m_isValid = false;
        }

        OriginParameter::OriginParameter(OriginParameter& other) 
        {
            this->m_isValid = other.m_isValid;
            this->m_origin = std::vector<double>(3);
            std::copy(other.m_origin.begin(), other.m_origin.end(), this->m_origin.begin());
        }

        OriginParameter::OriginParameter(OriginParameter const * const other)
        {
            this->m_isValid = other->m_isValid;
            this->m_origin = std::vector<double>(3);
            std::copy(other->m_origin.begin(), other->m_origin.end(), this->m_origin.begin());
        }

        OriginParameter& OriginParameter::operator=(const OriginParameter& other)
        {
            if(&other != this)
            {
                this->m_isValid = other.m_isValid;
                this->m_origin.clear();
                this->m_origin = std::vector<double>(3);
                std::copy(other.m_origin.begin(), other.m_origin.end(), this->m_origin.begin());
            }
            return *this;
        }

        std::string OriginParameter::getName() const
        {
            return parameterName();
        }

        bool OriginParameter::isValid() const
        {
            return m_isValid;
        }

        OriginParameter* OriginParameter::cloneImp() const
        {
            return new OriginParameter(m_origin.at(0), m_origin.at(1), m_origin.at(2));
        }

        std::auto_ptr<OriginParameter> OriginParameter::clone() const
        {
            return std::auto_ptr<OriginParameter>(cloneImp());
        }

        OriginParameter::~OriginParameter()
        {
        }

        double OriginParameter::getX() const
        {
            return m_origin.at(0);
        }

        double OriginParameter::getY() const
        {
            return m_origin.at(1);
        }

        double OriginParameter::getZ() const
        {
            return m_origin.at(2);
        }

        std::string OriginParameter::toXML() const
        {
            //TODO use proper DOM library for this
            char buffer[256];
            sprintf(buffer, "<Parameter><Type>OriginParameter</Type><Value>%.4f, %.4f, %.4f</Value></Parameter>", m_origin.at(0),  m_origin.at(1), m_origin.at(2));

            std::string xmlFragment = std::string(buffer);

            return xmlFragment;
        }
    }


}