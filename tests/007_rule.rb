require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Rule)
    assert Clips::Rule.constants.member?(:Creator)
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Rule::Creator.new }
    assert_raise(Clips::ArgumentError)  { Clips::Rule.new(3){}  }

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animal ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animal ?b))"
    end  
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) (child-of ?a ?b) => (assert (animal ?b)))"
  end
end
