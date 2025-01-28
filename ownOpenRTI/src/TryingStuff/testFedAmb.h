#ifndef TESTFEDAMB_H
#define TESTFEDAMB_H

#include <string>
#include <RTI/FederateAmbassador.h>

using namespace rti1516e;

class testFedAmb : public FederateAmbassador {
    public:
        double federateTime;
        double federateLookahead;

        bool isRegulating;
        bool isConstrained;
        bool isAdvancing;
        bool isAnnounced;
        bool isReadyToRun;
        
        testFedAmb();
        virtual ~testFedAmb() throw();

        // 1. Instance methods
        double convertTime(const LogicalTime& theTime);

        // 2. Synchrnization point methods
        virtual void synchronizationPointRegistrationSucceeded(const std::wstring& label);
        virtual void synchronizationPointRegistrationFailed(const std::wstring& label);
        virtual void announceSynchronizationPoint(const std::wstring& label, const VariableLengthData& tag);
        virtual void federationSynchronized(const std::wstring& label, const FederateHandleSet& failedToSyncSet);

        // 3. Time management methods
        virtual void timeRegulationEnabled(const LogicalTime& theFederateTime);
        virtual void timeConstrainedEnabled(const LogicalTime& theFederateTime);
        virtual void timeAdvanceGrant(const LogicalTime& theTime);

        // 4. Interaction methods
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                const ParameterHandleValueMap& theParameterValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder, TransportationType theType,
                SupplementalReceiveInfo theReceiveInfo);
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                const ParameterHandleValueMap& theParameterValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder, TransportationType theType,
                LogicalTime theTime, OrderType receivedOrder,
                SupplementalReceiveInfo theReceiveInfo);
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                const ParameterHandleValueMap& theParameterValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder, TransportationType theType,
                LogicalTime theTime, OrderType receivedOrder,
                MessageRetractionHandle retractionHandle,
                SupplementalReceiveInfo theReceiveInfo);
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                        const ParameterHandleValueMap& theParameterValues,
                                        const VariableLengthData& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        const RegionHandleSet& theSentRegionHandleSet,
                                        const SupplementalReceiveInfo& theReceiveInfo) override;
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                        const ParameterHandleValueMap& theParameterValues,
                                        const VariableLengthData& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        const RegionHandleSet& theSentRegionHandleSet,
                                        LogicalTime theTime,
                                        OrderType receivedOrder,
                                        const RegionHandleSet& theReceivedRegionHandleSet,
                                        const SupplementalReceiveInfo& theReceiveInfo) override;
        virtual void receiveInteraction(InteractionClassHandle theInteraction,
                                        const ParameterHandleValueMap& theParameterValues,
                                        const VariableLengthData& theUserSuppliedTag,
                                        OrderType sentOrder,
                                        TransportationType theType,
                                        const RegionHandleSet& theSentRegionHandleSet,
                                        LogicalTime theTime,
                                        OrderType receivedOrder,
                                        MessageRetractionHandle retractionHandle,
                                        const RegionHandleSet& theReceivedRegionHandleSet,
                                        const SupplementalReceiveInfo& theReceiveInfo) override;

        // 5. attribute methods
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                const AttributeHandleValueMap& theAttributeValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                TransportationType theType,
                SupplementalReflectInfo theReflectInfo);
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                const AttributeHandleValueMap& theAttributeValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                TransportationType theType,
                LogicalTime theTime,
                OrderType receivedOrder,
                SupplementalReflectInfo theReflectInfo);
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                const AttributeHandleValueMap& theAttributeValues,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                TransportationType theType,
                LogicalTime theTime,
                OrderType receivedOrder,
                MessageRetractionHandle retractionHandle,
                SupplementalReflectInfo theReflectInfo);
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                            const AttributeHandleValueMap& theAttributeValues,
                                            const VariableLengthData& theUserSuppliedTag,
                                            OrderType sentOrder,
                                            TransportationType theType,
                                            const RegionHandleSet& theSentRegionHandleSet,
                                            const SupplementalReflectInfo& theReflectInfo) override;
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                            const AttributeHandleValueMap& theAttributeValues,
                                            const VariableLengthData& theUserSuppliedTag,
                                            OrderType sentOrder,
                                            TransportationType theType,
                                            const RegionHandleSet& theSentRegionHandleSet,
                                            LogicalTime theTime,
                                            OrderType receivedOrder,
                                            const RegionHandleSet& theReceivedRegionHandleSet,
                                            const SupplementalReflectInfo& theReflectInfo) override;
        virtual void reflectAttributeValues(ObjectInstanceHandle theObject,
                                            const AttributeHandleValueMap& theAttributeValues,
                                            const VariableLengthData& theUserSuppliedTag,
                                            OrderType sentOrder,
                                            TransportationType theType,
                                            const RegionHandleSet& theSentRegionHandleSet,
                                            LogicalTime theTime,
                                            OrderType receivedOrder,
                                            MessageRetractionHandle retractionHandle,
                                            const RegionHandleSet& theReceivedRegionHandleSet,
                                            const SupplementalReflectInfo& theReflectInfo) override;

        // 6. Object methods
        virtual void discoverObjectInstance(ObjectInstanceHandle theObject,
                ObjectClassHandle theObjectClass,
                const std::wstring& theObjectInstanceName);
        virtual void discoverObjectInstance(ObjectInstanceHandle theObject,
                ObjectClassHandle theObjectClass,
                const std::wstring& theObjectInstanceName,
                FederateHandle producingFederate);
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                SupplementalRemoveInfo theRemoveInfo);
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                LogicalTime theTime,
                OrderType receivedOrder,
                SupplementalRemoveInfo theRemoveInfo);
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                const VariableLengthData& theUserSuppliedTag,
                OrderType sentOrder,
                LogicalTime theTime,
                OrderType receivedOrder,
                MessageRetractionHandle retractionHandle,
                SupplementalRemoveInfo theRemoveInfo);
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                          const VariableLengthData& theUserSuppliedTag,
                                          OrderType sentOrder,
                                          const RegionHandleSet& theSentRegionHandleSet,
                                          const SupplementalRemoveInfo& theRemoveInfo) override;
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                          const VariableLengthData& theUserSuppliedTag,
                                          OrderType sentOrder,
                                          const RegionHandleSet& theSentRegionHandleSet,
                                          LogicalTime theTime,
                                          OrderType receivedOrder,
                                          const RegionHandleSet& theReceivedRegionHandleSet,
                                          const SupplementalRemoveInfo& theRemoveInfo) override;
        virtual void removeObjectInstance(ObjectInstanceHandle theObject,
                                          const VariableLengthData& theUserSuppliedTag,
                                          OrderType sentOrder,
                                          const RegionHandleSet& theSentRegionHandleSet,
                                          LogicalTime theTime,
                                          OrderType receivedOrder,
                                          MessageRetractionHandle retractionHandle,
                                          const RegionHandleSet& theReceivedRegionHandleSet,
                                          const SupplementalRemoveInfo& theRemoveInfo) override;

    private:
        // Implementing pure virtual methods from FederateAmbassador
        virtual void connectionLost(const std::wstring& faultDescription) override;
        virtual void reportFederationExecutions(const FederationExecutionInformationVector& theFederationExecutionInformationList) override;
};

#endif